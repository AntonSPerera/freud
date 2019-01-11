// Copyright (c) 2010-2018 The Regents of the University of Michigan
// This file is from the freud project, released under the BSD 3-Clause License.

#include <algorithm>
#include <stdexcept>
#include <tbb/tbb.h>
#include <tuple>

#include "AABBQuery.h"
#include "LinkCell.h"

namespace freud { namespace locality {

AABBQuery::AABBQuery(): SpatialData(), m_rcut(0)
    {
    }

AABBQuery::AABBQuery(const box::Box &box, const vec3<float> *ref_points, unsigned int Nref):
    SpatialData(box, ref_points, Nref), m_rcut(0)
    {
    // Allocate memory and create image vectors
    setupTree(m_Nref, false);

    // Build the tree
    buildTree(m_ref_points, m_Nref);
    }

AABBQuery::~AABBQuery()
    {
    }


std::shared_ptr<SpatialDataIterator> AABBQuery::query(const vec3<float> point, unsigned int k, float r, float scale) const
    {
    return std::make_shared<AABBQueryIterator>(this, point, k, r, scale);
    }

//! Given a set of points, find all elements of this data structure
//  that are within a certain distance r.
std::shared_ptr<SpatialDataIterator> AABBQuery::query_ball(const vec3<float> point, float r) const
    {
    return std::make_shared<AABBQueryBallIterator>(this, point, r);
    }


void AABBQuery::compute(box::Box& box, float rcut,
        const vec3<float> *ref_points, unsigned int Nref,
        const vec3<float> *points, unsigned int Np,
        bool exclude_ii)
    {
    m_box = box;
    m_rcut = rcut;
    m_Ntotal = Nref + Np;

    // Allocate memory and create image vectors
    setupTree(Np, true);

    // Build the tree
    buildTree(points, Np);

    // Now walk the tree
    traverseTree(ref_points, Nref, points, Np, exclude_ii);
    }

void AABBQuery::setupTree(unsigned int Np, bool build_images)
    {
    m_aabbs.resize(Np);
    if (build_images)
        {
        // If we're using the new API, the image building has to occur on a per-query basis, so we don't bother to build the image list for the AABBQuery itself.
        updateImageVectors();
        }
    }

void AABBQuery::updateImageVectors()
    {
    vec3<float> nearest_plane_distance = m_box.getNearestPlaneDistance();
    vec3<bool> periodic = m_box.getPeriodic();
    float rmax = m_rcut;
    if ((periodic.x && nearest_plane_distance.x <= rmax * 2.0) ||
        (periodic.y && nearest_plane_distance.y <= rmax * 2.0) ||
        (!m_box.is2D() && periodic.z && nearest_plane_distance.z <= rmax * 2.0))
        {
        throw std::runtime_error("The AABBQuery rcut is too large for this box.");
        }

    // Now compute the image vectors
    // Each dimension increases by one power of 3
    unsigned int n_dim_periodic = (unsigned int)(periodic.x + periodic.y + (!m_box.is2D())*periodic.z);
    m_n_images = 1;
    for (unsigned int dim = 0; dim < n_dim_periodic; ++dim)
        {
        m_n_images *= 3;
        }

    // Reallocate memory if necessary
    if (m_n_images > m_image_list.size())
        {
        m_image_list.resize(m_n_images);
        }

    vec3<float> latt_a = vec3<float>(m_box.getLatticeVector(0));
    vec3<float> latt_b = vec3<float>(m_box.getLatticeVector(1));
    vec3<float> latt_c = vec3<float>(0.0, 0.0, 0.0);
    if (!m_box.is2D())
        {
        latt_c = vec3<float>(m_box.getLatticeVector(2));
        }

    // There is always at least 1 image, which we put as our first thing to look at
    m_image_list[0] = vec3<float>(0.0, 0.0, 0.0);

    // Iterate over all other combinations of images
    unsigned int n_images = 1;
    for (int i=-1; i <= 1 && n_images < m_n_images; ++i)
        {
        for (int j=-1; j <= 1 && n_images < m_n_images; ++j)
            {
            for (int k=-1; k <= 1 && n_images < m_n_images; ++k)
                {
                if (!(i == 0 && j == 0 && k == 0))
                    {
                    // Skip any periodic images if we don't have periodicity
                    if (i != 0 && !periodic.x) continue;
                    if (j != 0 && !periodic.y) continue;
                    if (k != 0 && (m_box.is2D() || !periodic.z)) continue;

                    m_image_list[n_images] = float(i) * latt_a + float(j) * latt_b + float(k) * latt_c;
                    ++n_images;
                    }
                }
            }
        }
    }

void AABBQuery::buildTree(const vec3<float> *points, unsigned int Np)
    {
    // Construct a point AABB for each point
    for (unsigned int i = 0; i < Np; ++i)
        {
        // Make a point AABB
        vec3<float> my_pos(points[i]);
        if (m_box.is2D())
            my_pos.z = 0;
        m_aabbs[i] = AABB(my_pos, i);
        }

    // Call the tree build routine, one tree per type
    m_aabb_tree.buildTree(m_aabbs.data(), Np);
    }

void AABBQuery::traverseTree(const vec3<float> *ref_points, unsigned int Nref,
        const vec3<float> *points, unsigned int Np, bool exclude_ii)
    {
    if (!Np)
        return;

    float r_cutsq = m_rcut * m_rcut;

    typedef std::vector<std::tuple<size_t, size_t, float> > BondVector;
    typedef std::vector<BondVector> BondVectorVector;
    typedef tbb::enumerable_thread_specific<BondVectorVector> ThreadBondVector;
    ThreadBondVector bond_vectors;

    // Loop over all reference points in parallel
    parallel_for(tbb::blocked_range<size_t>(0, Nref),
        [=, &bond_vectors] (const tbb::blocked_range<size_t> &r)
        {
        ThreadBondVector::reference bond_vector_vectors(bond_vectors.local());
        bond_vector_vectors.emplace_back();
        BondVector &bond_vector(bond_vector_vectors.back());

        // Loop over this thread's reference points
        for (size_t i(r.begin()); i != r.end(); ++i)
            {
            // Read in the position of i
            vec3<float> pos_i(ref_points[i]);
            if (m_box.is2D())
                {
                pos_i.z = 0;
                }

            // Loop over image vectors
            for (unsigned int cur_image = 0; cur_image < m_n_images; ++cur_image)
                {
                // Make an AABB for the image of this point
                vec3<float> pos_i_image = pos_i + m_image_list[cur_image];
                AABBSphere asphere = AABBSphere(pos_i_image, m_rcut);

                // Stackless traversal of the tree
                for (unsigned int cur_node_idx = 0;
                     cur_node_idx < m_aabb_tree.getNumNodes();
                     ++cur_node_idx)
                    {
                    if (overlap(m_aabb_tree.getNodeAABB(cur_node_idx), asphere))
                        {
                        if (m_aabb_tree.isNodeLeaf(cur_node_idx))
                            {
                            for (unsigned int cur_p = 0;
                                 cur_p < m_aabb_tree.getNodeNumParticles(cur_node_idx);
                                 ++cur_p)
                                {
                                // Neighbor j
                                const unsigned int j = m_aabb_tree.getNodeParticleTag(cur_node_idx, cur_p);

                                // Determine whether to skip self-interaction
                                if (exclude_ii && i == j)
                                    continue;

                                // Read in the position of j
                                vec3<float> pos_j(points[j]);
                                if (m_box.is2D())
                                    {
                                    pos_j.z = 0;
                                    }

                                // Compute distance
                                const vec3<float> drij = pos_j - pos_i_image;
                                const float dr_sq = dot(drij, drij);

                                if (dr_sq < r_cutsq)
                                    {
                                    bond_vector.emplace_back(i, j, 1);
                                    }
                                }
                            }
                        }
                    else
                        {
                        // Skip ahead
                        cur_node_idx += m_aabb_tree.getNodeSkip(cur_node_idx);
                        }
                    } // end stackless search
                } // end loop over images
            } // end loop over reference points
        });

    // Sort neighbors by particle i index
    tbb::flattened2d<ThreadBondVector> flat_bond_vector_groups = tbb::flatten2d(bond_vectors);
    BondVectorVector bond_vector_groups(flat_bond_vector_groups.begin(), flat_bond_vector_groups.end());
    tbb::parallel_sort(bond_vector_groups.begin(), bond_vector_groups.end(), compareFirstNeighborPairs);

    unsigned int num_bonds(0);
    for(BondVectorVector::const_iterator iter(bond_vector_groups.begin());
        iter != bond_vector_groups.end(); ++iter)
        num_bonds += iter->size();

    m_neighbor_list.resize(num_bonds);
    m_neighbor_list.setNumBonds(num_bonds, Nref, Np);

    size_t *neighbor_array(m_neighbor_list.getNeighbors());
    float *neighbor_weights(m_neighbor_list.getWeights());

    // Build nlist structure
    parallel_for(tbb::blocked_range<size_t>(0, bond_vector_groups.size()),
        [=, &bond_vector_groups] (const tbb::blocked_range<size_t> &r)
        {
        size_t bond(0);
        for (size_t group(0); group < r.begin(); ++group)
            bond += bond_vector_groups[group].size();

        for (size_t group(r.begin()); group < r.end(); ++group)
            {
            const BondVector &vec(bond_vector_groups[group]);
            for (BondVector::const_iterator iter(vec.begin());
                iter != vec.end(); ++iter, ++bond)
                {
                std::tie(neighbor_array[2*bond], neighbor_array[2*bond + 1],
                    neighbor_weights[bond]) = *iter;
                }
            }
        });
    }

void AABBIterator::updateImageVectors(float rmax)
    {
    box::Box box = m_spatial_data->getBox();
    vec3<float> nearest_plane_distance = box.getNearestPlaneDistance();
    vec3<bool> periodic = box.getPeriodic();
    if ((periodic.x && nearest_plane_distance.x <= rmax * 2.0) ||
        (periodic.y && nearest_plane_distance.y <= rmax * 2.0) ||
        (!box.is2D() && periodic.z && nearest_plane_distance.z <= rmax * 2.0))
        {
        throw std::runtime_error("The AABBData rcut is too large for this box.");
        }

    // Now compute the image vectors
    // Each dimension increases by one power of 3
    unsigned int n_dim_periodic = (unsigned int)(periodic.x + periodic.y + (!box.is2D())*periodic.z);
    m_n_images = 1;
    for (unsigned int dim = 0; dim < n_dim_periodic; ++dim)
        {
        m_n_images *= 3;
        }

    // Reallocate memory if necessary
    if (m_n_images > m_image_list.size())
        {
        m_image_list.resize(m_n_images);
        }

    vec3<float> latt_a = vec3<float>(box.getLatticeVector(0));
    vec3<float> latt_b = vec3<float>(box.getLatticeVector(1));
    vec3<float> latt_c = vec3<float>(0.0, 0.0, 0.0);
    if (!box.is2D())
        {
        latt_c = vec3<float>(box.getLatticeVector(2));
        }

    // There is always at least 1 image, which we put as our first thing to look at
    m_image_list[0] = vec3<float>(0.0, 0.0, 0.0);

    // Iterate over all other combinations of images
    unsigned int n_images = 1;
    for (int i=-1; i <= 1 && n_images < m_n_images; ++i)
        {
        for (int j=-1; j <= 1 && n_images < m_n_images; ++j)
            {
            for (int k=-1; k <= 1 && n_images < m_n_images; ++k)
                {
                if (!(i == 0 && j == 0 && k == 0))
                    {
                    // Skip any periodic images if we don't have periodicity
                    if (i != 0 && !periodic.x) continue;
                    if (j != 0 && !periodic.y) continue;
                    if (k != 0 && (box.is2D() || !periodic.z)) continue;

                    m_image_list[n_images] = float(i) * latt_a + float(j) * latt_b + float(k) * latt_c;
                    ++n_images;
                    }
                }
            }
        }
    }

NeighborPoint AABBQueryBallIterator::next()
    {
    float r_cutsq = m_r * m_r;

    // Read in the position of i
    vec3<float> pos_i(m_point);
    if (m_spatial_data->getBox().is2D())
        {
        pos_i.z = 0;
        }

    // Loop over image vectors
    while (cur_image < m_n_images)
        {
        // Make an AABB for the image of this point
        vec3<float> pos_i_image = pos_i + m_image_list[cur_image];
        AABBSphere asphere = AABBSphere(pos_i_image, m_r);

        // Stackless traversal of the tree
        while (cur_node_idx < m_aabb_data->m_aabb_tree.getNumNodes())
            {
            if (overlap(m_aabb_data->m_aabb_tree.getNodeAABB(cur_node_idx), asphere))
                {
                if (m_aabb_data->m_aabb_tree.isNodeLeaf(cur_node_idx))
                    {
                    while (cur_p < m_aabb_data->m_aabb_tree.getNodeNumParticles(cur_node_idx))
                        {
                        // Neighbor j
                        const unsigned int j = m_aabb_data->m_aabb_tree.getNodeParticleTag(cur_node_idx, cur_p);

                        // Read in the position of j
                        vec3<float> pos_j((*m_spatial_data)[j]);
                        if (m_spatial_data->getBox().is2D())
                            {
                            pos_j.z = 0;
                            }

                        // Compute distance
                        const vec3<float> drij = pos_j - pos_i_image;
                        const float dr_sq = dot(drij, drij);

                        if (dr_sq < r_cutsq)
                            {
                            // Return this one. Need to increment for the next call to the function, though.
                            cur_p++;
                            return NeighborPoint(j, sqrt(dr_sq));
                            }
                        cur_p++;
                        }
                    }
                }
            else
                {
                // Skip ahead
                cur_node_idx += m_aabb_data->m_aabb_tree.getNodeSkip(cur_node_idx);
                }
            cur_node_idx++;
            cur_p = 0;
            } // end stackless search
        cur_image++;
        cur_node_idx = 0;
        } // end loop over images

    m_finished = true;
    return SpatialData::ITERATOR_TERMINATOR;
    }

NeighborPoint AABBQueryIterator::next()
    {
    vec3<float> plane_distance = m_spatial_data->getBox().getNearestPlaneDistance();
    float min_plane_distance = std::min(std::min(plane_distance.x, plane_distance.y), plane_distance.z);

    if (m_finished)
        {
        return SpatialData::ITERATOR_TERMINATOR;
        }

    //TODO: Make sure to address case where there are NO NEIGHBORS (e.g. empty system). Currently I think that case will result in an infinite loop.

    // Make sure we're not calling next with neighbors left to return
    if (!m_current_neighbors.size())
        {
        // Continually perform ball queries until the termination conditions are met.
        while (true)
            {
            // Perform a ball query to get neighbors.
            m_current_neighbors.clear();
            std::shared_ptr<SpatialDataIterator> ball_it = m_spatial_data->query_ball(m_point, m_r);
            while(!ball_it->end())
                {
                m_current_neighbors.emplace_back(ball_it->next());
                }
            // Remove the last item, which is just the blank object
            m_current_neighbors.pop_back();

            // Break if there are enough neighbors
            if (m_current_neighbors.size() >= m_k)
                {
                break;
                }
            else
                {
                // Rescale, then check if we should break based on querying too
                // much space.
                m_r *= m_scale;
                if (m_r > min_plane_distance/2)
                    {
                    break;
                    }
                }
            }
        }

    // Now we return all the points found for the current point
    if (m_current_neighbors.size())
        {
        // Slightly inefficient because we're sorting every time, but
        // should be negligible unless we're querying for very large
        // numbers of nearest neighbors. Note that we use reverse iterators to
        // sort in descending order so that we can use pop_back to remove the
        // item from the vector before returning it.
        std::sort(m_current_neighbors.rbegin(), m_current_neighbors.rend());

        NeighborPoint ret_obj = m_current_neighbors.back();
        m_current_neighbors.pop_back();

        if (!m_current_neighbors.size())
            {
            m_finished = true;
            }
        return ret_obj;
        }
    }
}; }; // end namespace freud::locality
