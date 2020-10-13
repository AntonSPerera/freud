#ifndef NEIGHBOR_PER_POINT_ITERATOR_H
#define NEIGHBOR_PER_POINT_ITERATOR_H

#include "NeighborBond.h"

#ifdef LIBFREUD_EXPORTS
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC
#endif

/*! \file NeighborPerPointIterator.h
    \brief Defines interface for iterator looping over sets of neighbors.
*/


namespace freud { namespace locality {

//! Parent class for all per-point neighbor finding iteration.
/*! This is an abstract class that defines the API for finding neighbors on a
 *  per-point basis. Any class that wishes to provide a way to iterate over sets
 *  of neighbors should subclass this iterator and provide the logic in the
 *  subclass for iterating over the neighbors of specific points. The per-point
 *  iteration is at the core of the neighbor querying infrastructure in freud,
 *  because it allows all relevant logic on finding neighbors to be encoded and
 *  provides a base unit of parallelism (it is safe to create multiple
 *  NeighborPerPointIterator iterators in parallel for different points, and
 *  then accumulate all neighbors at the end.
 *
 *  All subclasses must define the next() method and end() methods
 *  appropriately. The next() method is the primary mode of interaction with
 *  the iterator, and allows looping through the iterator. The end() method
 *  defines when the iteration is complete and no more neighbors remain to be
 *  found. Due to the fact that there is no way to know when iteration is
 *  complete until all relevant points are actually checked (irrespective of
 *  the underlying data structure), the end() method will not return true until
 *  the next method reaches the end of control flow at least once without
 *  finding a next neighbor. As a result, the next() method is required to
 *  return NeighborPerPointIterator::ITERATOR_TERMINATOR on all calls
 *  after the last neighbor is found in order to guarantee that the correct set
 *  of neighbors is considered.
 */
class DECLSPEC NeighborPerPointIterator
{
public:
    //! Nullary constructor for Cython
    NeighborPerPointIterator();

    //! Constructor
    NeighborPerPointIterator(unsigned int query_point_idx);

    //! Empty Destructor
    virtual ~NeighborPerPointIterator();

    //! Indicate when done.
    virtual bool end() = 0;

    //! Get the next element.
    virtual NeighborBond next() = 0;

    static const NeighborBond ITERATOR_TERMINATOR; //!< The object returned when iteration is complete.

protected:
    unsigned int m_query_point_idx; //!< The index of the query point.
};

}; }; // end namespace freud::locality

#endif // NEIGHBOR_PER_POINT_ITERATOR_H
