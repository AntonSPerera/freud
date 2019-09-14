// Copyright (c) 2010-2019 The Regents of the University of Michigan
// This file is from the freud project, released under the BSD 3-Clause License.

#include <stdexcept>
#ifdef __SSE2__
#include <emmintrin.h>
#endif

#include "RDF.h"

/*! \file RDF.cc
    \brief Routines for computing radial density functions.
*/

namespace freud { namespace density {

RDF::RDF(unsigned int bins, float r_max, float r_min) : m_box(box::Box()), m_frame_counter(0), m_n_points(0),
    m_n_query_points(0), m_reduce(true), m_r_max(r_max), m_r_min(r_min), m_bins(bins)
{
    if (bins <= 0)
        throw std::invalid_argument("RDF requires a positive number of bins.");
    if (r_max <= 0.0f)
        throw std::invalid_argument("RDF requires r_max to be positive.");
    if (r_max <= r_min)
        throw std::invalid_argument("RDF requires that r_max must be greater than r_min.");

    // Construct the Histogram object that will be used to keep track of counts of bond distances found.
    util::Histogram::Axes axes;
    axes.push_back(std::make_shared<util::RegularAxis>(m_bins, m_r_min, m_r_max));
    m_histogram = util::Histogram(axes);
    m_local_histograms = util::Histogram::ThreadLocalHistogram(m_histogram);

    // Precompute the cell volumes to speed up later calculations.
    m_vol_array2D.prepare(m_bins);
    m_vol_array3D.prepare(m_bins);
    float volume_prefactor = (float(4.0)/float(3.0))*M_PI;
    std::vector<float> bin_boundaries = getBins();

    for (unsigned int i = 0; i < m_bins; i++)
    {
        float r = bin_boundaries[i];
        float nextr = bin_boundaries[i+1];
        m_vol_array2D[i] = M_PI * (nextr * nextr - r * r);
        m_vol_array3D[i] = volume_prefactor * (nextr * nextr * nextr - r * r * r);
    }
}

void RDF::reduce()
{
    m_pcf.prepare(m_bins);
    m_histogram.reset();
    m_N_r.prepare(m_bins);

    // Define prefactors with appropriate types to simplify and speed later code.
    float ndens = float(m_n_query_points) / m_box.getVolume();
    float np = static_cast<float>(m_n_points);
    float prefactor = float(1.0)/(np*ndens*m_frame_counter);

    util::ManagedArray<float> vol_array = m_box.is2D() ? m_vol_array2D : m_vol_array3D;
    m_histogram.reduceOverThreadsPerBin(m_local_histograms,
            [this, &prefactor, &vol_array] (size_t i) {
            m_pcf[i] = m_histogram[i] * prefactor / vol_array[i];
            });

    // The accumulation of the cumulative density must be performed in
    // sequence, so it is done after the reduction.
    prefactor = float(1.0)/(np*m_frame_counter);
    m_N_r[0] = m_histogram[0] * prefactor;
    for (unsigned int i = 1; i < m_bins; i++)
    {
        m_N_r[i] = m_N_r[i-1] + m_histogram[i] * prefactor;
    }
}

void RDF::reset()
{
    m_local_histograms.reset();
    this->m_frame_counter = 0;
    this->m_reduce = true;
}

void RDF::accumulate(const freud::locality::NeighborQuery* neighbor_query,
                    const vec3<float>* query_points, unsigned int n_query_points,
                    const freud::locality::NeighborList* nlist, freud::locality::QueryArgs qargs)
{
    m_box = neighbor_query->getBox();
    locality::loopOverNeighbors(neighbor_query, query_points, n_query_points, qargs, nlist,
        [=](const freud::locality::NeighborBond& neighbor_bond) {
        if (neighbor_bond.distance < m_r_max && neighbor_bond.distance > m_r_min)
        {
            m_local_histograms(neighbor_bond.distance);
        }
    });
    m_frame_counter++;
    m_n_points = neighbor_query->getNPoints();
    m_n_query_points = n_query_points;
    m_reduce = true;
}

}; }; // end namespace freud::density
