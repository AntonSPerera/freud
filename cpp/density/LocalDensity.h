// Copyright (c) 2010-2019 The Regents of the University of Michigan
// This file is from the freud project, released under the BSD 3-Clause License.

#ifndef LOCAL_DENSITY_H
#define LOCAL_DENSITY_H

#include <memory>
#include <tbb/tbb.h>

#include "Box.h"
#include "NeighborList.h"
#include "NeighborQuery.h"
#include "VectorMath.h"
#include "ManagedArray.h"

/*! \file LocalDensity.h
    \brief Routines for computing local density around a point.
*/

namespace freud { namespace density {

//! Compute the local density at each point
/*!
 */
class LocalDensity
{
public:
    //! Constructor
    LocalDensity(float r_max, float volume, float diameter);

    //! Destructor
    ~LocalDensity();

    //! Get the simulation box
    const box::Box& getBox() const
    {
        return m_box;
    }

    //! Compute the local density
    void compute(const freud::locality::NeighborQuery* neighbor_query, const vec3<float>* query_points,
                 unsigned int n_query_points, const freud::locality::NeighborList* nlist,
                 freud::locality::QueryArgs qargs);

    //! Get the number of reference particles
    unsigned int getNPoints();

    //! Get a reference to the last computed density
    const util::ManagedArray<float> &getDensity();

    //! Get a reference to the last computed number of neighbors
    const util::ManagedArray<float> &getNumNeighbors();

private:
    box::Box m_box;       //!< Simulation box where the particles belong
    float m_r_max;         //!< Maximum neighbor distance
    float m_volume;       //!< Volume (area in 2d) of a single particle
    float m_diameter;     //!< Diameter of the particles
    unsigned int m_n_points; //!< Last number of points computed

    util::ManagedArray<float> m_density_array;       //!< density array computed
    util::ManagedArray<float> m_num_neighbors_array; //!< number of neighbors array computed
};

}; }; // end namespace freud::density

#endif // LOCAL_DENSITY_H
