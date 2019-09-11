// Copyright (c) 2010-2019 The Regents of the University of Michigan
// This file is from the freud project, released under the BSD 3-Clause License.

#include <complex>
#include <stdexcept>

#include "LocalDensity.h"
#include "NeighborComputeFunctional.h"

using namespace std;
using namespace tbb;

/*! \file LocalDensity.cc
    \brief Routines for computing local density around a point.
*/

namespace freud { namespace density {

LocalDensity::LocalDensity(float r_max, float volume, float diameter)
    : m_box(box::Box()), m_r_max(r_max), m_volume(volume), m_diameter(diameter), m_n_points(0)
{}

LocalDensity::~LocalDensity() {}

void LocalDensity::compute(const freud::locality::NeighborQuery* neighbor_query, const vec3<float>* query_points,
                 unsigned int n_query_points, const freud::locality::NeighborList* nlist,
                 freud::locality::QueryArgs qargs)
{
    m_box = neighbor_query->getBox();

    unsigned int n_points = neighbor_query->getNPoints();

    m_density_array.prepare(n_points);
    m_num_neighbors_array.prepare(n_points);

    float area = M_PI * m_r_max * m_r_max;
    float volume = 4.0f/3.0f * M_PI * m_r_max * m_r_max * m_r_max;
    // compute the local density
    freud::locality::loopOverNeighborsIterator(neighbor_query, query_points, n_query_points, qargs, nlist,
    [=](size_t i, std::shared_ptr<freud::locality::NeighborPerPointIterator> ppiter)
    {
        float num_neighbors = 0;
        for(freud::locality::NeighborBond nb = ppiter->next(); !ppiter->end(); nb = ppiter->next())
            {
            // count particles that are fully in the r_max sphere
            if (nb.distance < (m_r_max - m_diameter/2.0f))
            {
              num_neighbors += 1.0f;
            }
            else
            {
              // partially count particles that intersect the r_max sphere
              // this is not particularly accurate for a single particle, but works well on average for
              // lots of them. It smooths out the neighbor count distributions and avoids noisy spikes
              // that obscure data
              num_neighbors += 1.0f + (m_r_max - (nb.distance + m_diameter/2.0f)) / m_diameter;
            }
            m_num_neighbors_array[i] = num_neighbors;
            if (m_box.is2D())
              {
              // local density is area of particles divided by the area of the circle
              m_density_array[i] = (m_volume * m_num_neighbors_array[i]) / area;
              }
            else
              {
              // local density is volume of particles divided by the volume of the sphere
              m_density_array[i] = (m_volume * m_num_neighbors_array[i]) / volume;
            }
        }
    });
    m_n_points = n_points;
}

unsigned int LocalDensity::getNPoints()
{
    return m_n_points;
}

//! Get a reference to the last computed density
const util::ManagedArray<float> &LocalDensity::getDensity()
{
    return m_density_array;
}

//! Get a reference to the last computed number of neighbors
const util::ManagedArray<float> &LocalDensity::getNumNeighbors()
{
    return m_num_neighbors_array;
}

}; }; // end namespace freud::density
