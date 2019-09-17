// Copyright (c) 2010-2019 The Regents of the University of Michigan
// This file is from the freud project, released under the BSD 3-Clause License.

#include <stdexcept>

#include "PMFTR12.h"

using namespace std;
using namespace tbb;

/*! \file PMFTR12.cc
    \brief Routines for computing potential of mean force and torque in R12 coordinates
*/

namespace freud { namespace pmft {

PMFTR12::PMFTR12(float r_max, unsigned int n_r, unsigned int n_t1, unsigned int n_t2)
    : PMFT(), m_t1_max(2.0 * M_PI), m_t2_max(2.0 * M_PI), m_n_r(n_r), m_n_t1(n_t1),
      m_n_t2(n_t2)
{
    if (n_r < 1)
        throw invalid_argument("PMFTR12 requires at least 1 bin in R.");
    if (n_t1 < 1)
        throw invalid_argument("PMFTR12 requires at least 1 bin in T1.");
    if (n_t2 < 1)
        throw invalid_argument("PMFTR12 requires at least 1 bin in T2.");
    if (r_max < 0.0f)
        throw invalid_argument("PMFTR12 requires that r_max must be positive.");
    // calculate dr, dt1, dt2
    m_r_max = r_max;
    m_dr = m_r_max / float(m_n_r);
    m_dt1 = m_t1_max / float(m_n_t1);
    m_dt2 = m_t2_max / float(m_n_t2);

    if (m_dr > r_max)
        throw invalid_argument("PMFTR12 requires that dr is less than or equal to r_max.");
    if (m_dt1 > m_t1_max)
        throw invalid_argument("PMFTR12 requires that dt1 is less than or equal to t1_max.");
    if (m_dt2 > m_t2_max)
        throw invalid_argument("PMFTR12 requires that dt2 is less than or equal to t2_max.");

    // precompute the bin center positions for r
    m_r_array = precomputeArrayGeneral(m_n_r, m_dr, [](float r, float nextr) {
        return 2.0f / 3.0f * (nextr * nextr * nextr - r * r * r) / (nextr * nextr - r * r);
    });

    // calculate the jacobian array; computed as the inverse for faster use later
    m_inv_jacobian_array.prepare({m_n_r, m_n_t1, m_n_t2});
    for (unsigned int i = 0; i < m_n_r; i++)
    {
        float r = m_r_array[i];
        for (unsigned int j = 0; j < m_n_t1; j++)
        {
            for (unsigned int k = 0; k < m_n_t2; k++)
            {
                m_inv_jacobian_array(i, j, k) = (float) 1.0 / (r * m_dr * m_dt1 * m_dt2);
            }
        }
    }

    // precompute the bin center positions for T1
    m_t1_array = precomputeAxisBinCenter(m_n_t1, m_dt1, 0);

    // precompute the bin center positions for T2
    m_t2_array = precomputeAxisBinCenter(m_n_t2, m_dt2, 0);

    // create and populate the pcf_array
    m_pcf_array.prepare({m_n_r, m_n_t1, m_n_t2});

    // Construct the Histogram object that will be used to keep track of counts of bond distances found.
    util::Histogram::Axes axes;
    axes.push_back(std::make_shared<util::RegularAxis>(n_r, 0, m_r_max));
    axes.push_back(std::make_shared<util::RegularAxis>(n_t1, 0, m_t1_max));
    axes.push_back(std::make_shared<util::RegularAxis>(n_t2, 0, m_t2_max));
    m_histogram = util::Histogram(axes);
    m_local_histograms = util::Histogram::ThreadLocalHistogram(m_histogram);
}

//! \internal
//! helper function to reduce the thread specific arrays into one array
void PMFTR12::reducePCF()
{
    reduce([this](size_t i) { return m_inv_jacobian_array[i]; });
}

void PMFTR12::accumulate(const locality::NeighborQuery* neighbor_query,
                         float* orientations, vec3<float>* query_points,
                         float* query_orientations, unsigned int n_p,
                         const locality::NeighborList* nlist, freud::locality::QueryArgs qargs)
{
    accumulateGeneral(neighbor_query, query_points, n_p, nlist, qargs,
        [=](const freud::locality::NeighborBond& neighbor_bond) {
        vec3<float> ref = neighbor_query->getPoints()[neighbor_bond.ref_id];
        vec3<float> delta = m_box.wrap(query_points[neighbor_bond.id] - ref);
        if (neighbor_bond.distance < m_r_max)
        {
            // calculate angles
            float d_theta1 = atan2(delta.y, delta.x);
            float d_theta2 = atan2(-delta.y, -delta.x);
            float t1 = orientations[neighbor_bond.ref_id] - d_theta1;
            float t2 = query_orientations[neighbor_bond.id] - d_theta2;
            // make sure that t1, t2 are bounded between 0 and 2PI
            t1 = fmod(t1, 2 * M_PI);
            if (t1 < 0)
            {
                t1 += 2 * M_PI;
            }
            t2 = fmod(t2, 2 * M_PI);
            if (t2 < 0)
            {
                t2 += 2 * M_PI;
            }
            m_local_histograms(neighbor_bond.distance, t1, t2);
        }
    });
}

}; }; // end namespace freud::pmft
