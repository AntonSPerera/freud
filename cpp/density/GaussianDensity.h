// Copyright (c) 2010-2019 The Regents of the University of Michigan
// This file is from the freud project, released under the BSD 3-Clause License.

#ifndef GAUSSIAN_DENSITY_H
#define GAUSSIAN_DENSITY_H

#include <memory>

#include "Box.h"
#include "Index1D.h"
#include "ThreadStorage.h"
#include "VectorMath.h"

/*! \file GaussianDensity.h
    \brief Routines for computing Gaussian smeared densities from points.
*/

namespace freud { namespace density {

//! Computes the the density of a system on a grid.
/*! Replaces particle positions with a gaussian and calculates the
        contribution from the grid based upon the the distance of the grid cell
        from the center of the Gaussian.
*/
class GaussianDensity
{
public:
    //! Constructor
    GaussianDensity(vec3<unsigned int> width, float r_cut, float sigma);

    // Destructor
    ~GaussianDensity() {}

    //! Get the simulation box
    const box::Box& getBox() const
    {
        return m_box;
    }

    //! Get the width of the gaussian distributions.
    float getSigma() const
    {
        return m_sigma;
    }

    //! Reset the gaussian array to all zeros
    void reset();

    //! \internal
    //! helper function to reduce the thread specific arrays into one array
    void reduce();

    //! Compute the Density
    void compute(const box::Box& box, const vec3<float>* points, unsigned int n_points);

    //! Get a reference to the last computed Density
    std::shared_ptr<float> getDensity();

    vec3<unsigned int> getWidth();

private:
    box::Box m_box;                               //!< Simulation box where the particles belong
    vec3<unsigned int> m_width;                   //!< Num of bins on each side of the cube
    float m_rcut;                                 //!< Max r at which to compute density
    float m_sigma;                                //!< Variance
    Index3D m_bi;                                 //!< Bin indexer
    bool m_reduce;                                //!< Whether arrays need to be reduced across threads

    std::shared_ptr<float> m_density_array; //! computed density array
    util::ThreadStorage<float> m_local_bin_counts;
};

}; }; // end namespace freud::density

#endif // GAUSSIAN_DENSITY_H
