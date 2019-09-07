// Copyright (c) 2010-2019 The Regents of the University of Michigan
// This file is from the freud project, released under the BSD 3-Clause License.

#include <complex>
#include <limits>
#include <stdexcept>
#ifdef __SSE2__
#include <emmintrin.h>
#endif

#include "Index1D.h"
#include "NematicOrderParameter.h"
#include "diagonalize.h"

using namespace std;
using namespace tbb;

/*! \file NematicOrderParameter.h
    \brief Compute the nematic order parameter for each particle
*/

namespace freud { namespace order {

// m_u is the molecular axis, normalized to a unit vector
NematicOrderParameter::NematicOrderParameter(vec3<float> u)
    : m_n(0), m_u(u / sqrt(dot(u, u)))
{}

float NematicOrderParameter::getNematicOrderParameter()
{
    return m_nematic_order_parameter;
}

const util::ManagedArray<float> &NematicOrderParameter::getParticleTensor()
{
    return m_particle_tensor;
}

const util::ManagedArray<float> &NematicOrderParameter::getNematicTensor()
{
    return m_nematic_tensor;
}

unsigned int NematicOrderParameter::getNumParticles()
{
    return m_n;
}

vec3<float> NematicOrderParameter::getNematicDirector()
{
    return m_nematic_director;
}

void NematicOrderParameter::compute(quat<float>* orientations, unsigned int n)
{
    m_n = n;
    m_particle_tensor.prepare({m_n, 3, 3});

    // calculate per-particle tensor
    parallel_for(blocked_range<size_t>(0, n), [=](const blocked_range<size_t>& r) {

        for (size_t i = r.begin(); i != r.end(); i++)
        {
            // get the director of the particle
            quat<float> q = orientations[i];
            vec3<float> u_i = rotate(q, m_u);

            util::ManagedArray<float> Q_ab({3, 3});

            Q_ab(0, 0) = 1.5f * u_i.x * u_i.x - 0.5f;
            Q_ab(0, 1) = 1.5f * u_i.x * u_i.y;
            Q_ab(0, 2) = 1.5f * u_i.x * u_i.z;
            Q_ab(1, 0) = 1.5f * u_i.y * u_i.x;
            Q_ab(1, 1) = 1.5f * u_i.y * u_i.y - 0.5f;
            Q_ab(1, 2) = 1.5f * u_i.y * u_i.z;
            Q_ab(2, 0) = 1.5f * u_i.z * u_i.x;
            Q_ab(2, 1) = 1.5f * u_i.z * u_i.y;
            Q_ab(2, 2) = 1.5f * u_i.z * u_i.z - 0.5f;

            // Set the values. The per-particle array is used so that both
            // this loop and the reduction can be done in parallel afterwards
            for (unsigned int j = 0; j < 3; j++)
                for (unsigned int k = 0; k < 3; k++)
                    m_particle_tensor(i, j, k) += Q_ab(j, k);
        }
    });

    // https://stackoverflow.com/questions/9399929/parallel-reduction-of-an-array-on-cpu
    struct reduce_matrix
    {
        util::ManagedArray<float> y_;
        const util::ManagedArray<float> m_; // reference to array of matrices per-particle

        reduce_matrix(const util::ManagedArray<float> m) : m_(m)
        {
            y_.prepare({3, 3});
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    y_(i, j) = 0.0; // prepare for accumulation
        }

        // splitting constructor required by TBB
        reduce_matrix(reduce_matrix& rm, tbb::split) : m_(rm.m_)
        {
            y_.prepare({3, 3});
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    y_(i, j) = 0.0;
        }

        // adding the elements
        void operator()(const tbb::blocked_range<unsigned int>& r)
        {
            for (unsigned int n = r.begin(); n < r.end(); ++n)
                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                        y_(i, j) += m_(n, i, j);
        }

        // reduce computations in two matrices
        void join(reduce_matrix& rm)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    y_(i, j) += rm.y_(i, j);
        }
    };

    // now calculate the sum of Q_ab's
    reduce_matrix matrix(m_particle_tensor);

    parallel_reduce(blocked_range<unsigned int>(0, m_n), matrix);

    // set the averaged Q_ab
    m_nematic_tensor.prepare({3, 3});
    for (unsigned int i = 0; i < 9; ++i)
        m_nematic_tensor[i] = matrix.y_[i] / m_n;

    // the order parameter is the eigenvector belonging to the largest eigenvalue
    Index2D a_i = Index2D(3);
    float evec[9];
    float eval[3];
    freud::util::diagonalize33SymmetricMatrix(m_nematic_tensor, eval, evec);
    m_nematic_director = vec3<Scalar>(evec[a_i(0, 2)], evec[a_i(1, 2)], evec[a_i(2, 2)]);
    m_nematic_order_parameter = eval[2];
}

}; }; // end namespace freud::order
