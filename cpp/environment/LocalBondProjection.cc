// Copyright (c) 2010-2019 The Regents of the University of Michigan
// This file is part of the freud project, released under the BSD 3-Clause License.

#include <cassert>
#include <stdexcept>

#include "LocalBondProjection.h"

using namespace std;
using namespace tbb;

/*! \file LocalBondProjection.h
    \brief Compute the projection of nearest neighbor bonds for each particle onto some
    set of reference vectors, defined in the particles' local reference frame.
*/

namespace freud { namespace environment {

LocalBondProjection::LocalBondProjection() : m_n_query_points(0), m_n_points(0), m_n_proj(0), m_n_equiv(0), m_tot_num_neigh(0)
{}

LocalBondProjection::~LocalBondProjection() {}

// The set of all equivalent quaternions equiv_qs is the set that takes the particle as it
// is defined to some global reference orientation. Thus, to be safe, we must include
// a rotation by qconst as defined below when doing the calculation.
// IMPT: equiv_qs does NOT have to include both q and -q, for all included quaternions.
// Rather, equiv_qs SHOULD contain the identity, and have the same length as the order of
// the chiral symmetry group of the particle shape.
// q and -q effect the same rotation on vectors, and here we just use equiv_quats to
// find all symmetrically equivalent vectors to proj_vec.
float computeMaxProjection(const vec3<float> proj_vec, const vec3<float> local_bond,
                           const quat<float>* equiv_qs, unsigned int n_equiv)
{
    quat<float> qconst = equiv_qs[0];

    // start with the reference vector before it has been rotated by equivalent quaternions
    vec3<float> max_proj_vec = proj_vec;
    float max_proj = dot(proj_vec, local_bond);

    // loop through all equivalent rotations and see if they have a larger projection onto local_bond
    for (unsigned int i = 0; i < n_equiv; i++)
    {
        quat<float> qe = equiv_qs[i];
        // here we undo a rotation represented by one of the equivalent orientations
        quat<float> qtest = conj(qconst) * qe;
        vec3<float> equiv_proj_vec = rotate(qtest, proj_vec);

        float proj_test = dot(equiv_proj_vec, local_bond);

        if (proj_test > max_proj)
        {
            max_proj = proj_test;
            max_proj_vec = equiv_proj_vec;
        }
    }

    return max_proj;
}

void LocalBondProjection::compute(box::Box& box, 
    const vec3<float>* proj_vecs,  unsigned int n_proj,
    const vec3<float>* points, const quat<float>* orientations, unsigned int n_points,
    const vec3<float>* query_points, unsigned int n_query_points,
    const quat<float>* equiv_quats, unsigned int n_equiv,
    const freud::locality::NeighborList* nlist)
{
    assert(query_points);
    assert(points);
    assert(orientations);
    assert(equiv_quats);
    assert(proj_vecs);
    assert(n_query_points > 0);
    assert(n_points > 0);
    assert(n_equiv > 0);
    assert(n_proj > 0);

    nlist->validate(n_query_points, n_points);
    const size_t* neighbor_list(nlist->getNeighbors());
    // Get the maximum total number of bonds in the neighbor list
    const size_t tot_num_neigh = nlist->getNumBonds();

    // reallocate the output array if it is not the right size
    if (tot_num_neigh != m_tot_num_neigh || n_proj != m_n_proj)
    {
        m_local_bond_proj
            = std::shared_ptr<float>(new float[tot_num_neigh * n_proj], std::default_delete<float[]>());
        m_local_bond_proj_norm
            = std::shared_ptr<float>(new float[tot_num_neigh * n_proj], std::default_delete<float[]>());
    }

    // compute the order parameter
    parallel_for(blocked_range<size_t>(0, n_query_points), [=](const blocked_range<size_t>& r) {
        size_t bond(nlist->find_first_index(r.begin()));
        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            
            for (; bond < tot_num_neigh && neighbor_list[2 * bond] == i; ++bond)
            {
                const size_t j(neighbor_list[2 * bond + 1]);

                // compute bond vector between the two particles
                vec3<float> delta = box.wrap(query_points[i] - points[j]);
                vec3<float> local_bond(delta);
                // rotate bond vector into the local frame of particle p
                local_bond = rotate(conj(orientations[j]), local_bond);
                // store the length of this local bond
                float local_bond_len = sqrt(dot(local_bond, local_bond));

                for (unsigned int k = 0; k < n_proj; k++)
                {
                    vec3<float> proj_vec = proj_vecs[k];
                    float max_proj = computeMaxProjection(proj_vec, local_bond, equiv_quats, n_equiv);
                    m_local_bond_proj.get()[bond * n_proj + k] = max_proj;
                    m_local_bond_proj_norm.get()[bond * n_proj + k] = max_proj / local_bond_len;
                }
            }
        }
    });

    // save the last computed box
    m_box = box;
    // save the last computed number of particles
    m_n_query_points = n_query_points;
    // save the last computed number of reference particles
    m_n_points = n_points;
    // save the last computed number of equivalent quaternions
    m_n_equiv = n_equiv;
    // save the last computed number of reference projection vectors
    m_n_proj = n_proj;
    // save the last computed number of total bonds
    m_tot_num_neigh = tot_num_neigh;
}

}; }; // end namespace freud::environment
