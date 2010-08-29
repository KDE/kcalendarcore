/*
  This file is part of the kcalcore library.

  Copyright (c) 2006 David Jarvie <software@astrojar.org.uk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the Sortable List class.

  @author David Jarvie \<software@astrojar.org.uk\>.
*/

#ifndef KCALCORE_SORTABLELIST_H
#define KCALCORE_SORTABLELIST_H

#include <QtCore/QList>
#include <QtCore/QtAlgorithms>

namespace KCalCore {

//@cond PRIVATE
template <class T>
void qSortUnique( QList<T> &list )
{
  if ( list.count() <= 1 ) {
    return;
  }
  qSort( list );
  typename QList<T>::iterator prev = list.begin();
  for ( typename QList<T>::iterator it = prev + 1;  it != list.end();  ++it ) {
    if ( *it == *prev ) {
      // Found two equal values. Search for any further equal values and remove
      // them all together for efficiency.
      while ( ++it != list.end()  &&  *it == *prev ) ;
      prev = it = list.erase( prev + 1, it );
      if ( it == list.end() ) {
        break;
      }
    } else {
      prev = it;
    }
  }
}
//@endcond

/**
  @brief A QList which can be sorted

  For a QList is capable of being sorted, SortedList provides additional
  optimized methods which can be used when the list is sorted and has no
  duplicate entries.

  Because SortableList has no data members, an object may be referred to
  interchangeably as either a QList or SortableList. Just bear in mind that
  the results of the SortableList methods are undefined when the list is
  unsorted or contains duplicate entries.

  To sort the list and remove duplicate entries, thereby allowing use of
  other SortableList methods, use sortUnique(). Once sortUnique() has been
  called, use findSorted(), containsSorted() and removeSorted() in preference
  to QList::indexOf(), QList::contains() and QList::removeAll(). Use findLE(),
  findLT(), findGE(), findGT() to find the index to the nearest value in the
  list which is <=, <, >= or > a given value. To add a value to the list,
  use insertSorted() in preference to insert(), append(), prepend(),
  operator<<() or operator+=().

  @author David Jarvie \<software@astrojar.org.uk\>.
*/
template <class T>
class SortableList : public QList<T>
{
  public:
    /**
      Constructs an empty sortable list.
    */
    SortableList() {}

    /**
      Constructs a sortable list by copying another one.

      @param list is the list to copy.
    */
    SortableList( const QList<T> &list ) : QList<T>( list ) {}   // implicit conversion

    /**
      Return whether the list contains value @p value. The list must be sorted;
      if not, the result is undefined.
      When the list is sorted, use this optimised method in preference to
      QList<T>::contains().

      @param value is the value to find.
      @return true if list contains @p value; false otherwise.
    */
    bool containsSorted( const T &value ) const  { return findSorted( value ) >= 0; }

    /**
      Search the list for the item equal to @p value. The list must be sorted;
      if not, the result is undefined.
      When the list is sorted, use this optimised method in preference to
      QList<T>::indexOf().

      @param value is the value to find.
      @param start is the start index for search (default is from beginning).
      @return index to item in list, or -1 if @p value not found in the list.
    */
    int findSorted( const T &value, int start = 0 ) const;

    /**
      Search the list for the last item <= @p value. The list must be sorted;
      if not, the result is undefined.

      @param value is the value to find.
      @param start is the start index for search (default is from beginning).
      @return index to item in list, or -1 if @p value < first value in the list.
    */
    int findLE( const T &value, int start = 0 ) const;

    /**
      Search the list for the last item < @p value. The list must be sorted;
      if not, the result is undefined.

      @param value is the value to find.
      @param start is the start index for search (default is from beginning).
      @return index to item in list, or -1 if @p value <= first value in the list.
    */
    int findLT( const T &value, int start = 0 ) const;

    /**
      Search the list for the first item >= @p value. The list must be sorted;
      if not, the result is undefined.

      @param value is the value to find.
      @param start is the start index for search (default is from beginning).
      @return index to item in list, or -1 if @p value > last value in the list.
    */
    int findGE( const T &value, int start = 0 ) const;

    /**
      Search the list for the first item > @p value. The list must be sorted;
      if not, the result is undefined.

      @param value is the value to find.
      @param start is the start index for search (default is from beginning).
      @return index to item in list, or -1 if @p value >= last value in the list.
    */
    int findGT( const T &value, int start = 0 ) const;

    /**
      Insert a value in the list, in correct sorted order. If the same value
      is already in the list, no change is made.

      The list must already be sorted before calling this method; otherwise
      the result is undefined.

      @param value is the value to insert.
      @return index to inserted item in list, or to the pre-existing entry
      equal to @p value.
    */
    int insertSorted( const T &value );

    /**
      Remove value @p value from the list. The list must be sorted.
      When the list is sorted, use this optimised method in preference to
      QList<T>::removeAll().

      @param value is the value to remove.
      @param start is the start index for search (default is from beginning).
      @return index to removed value, or -1 if not found.
    */
    int removeSorted( const T &value, int start = 0 );

    /**
      Sort the list. Any duplicate values are removed.
    */
    void sortUnique()  { qSortUnique( *this ); }
};

template <class T>
int SortableList<T>::findSorted( const T &value, int start ) const
{
  // Do a binary search to find the item == value
  int st = start - 1;
  int end = QList<T>::count();
  while ( end - st > 1 ) {
    int i = ( st + end ) / 2;
    if ( value < QList<T>::at(i) ) {
      end = i;
    } else {
      st = i;
    }
  }
  return ( end > start && value == QList<T>::at(st) ) ? st : -1;
}

template <class T>
int SortableList<T>::findLE( const T &value, int start ) const
{
  // Do a binary search to find the last item <= value
  int st = start - 1;
  int end = QList<T>::count();
  while ( end - st > 1 ) {
    int i = ( st + end ) / 2;
    if ( value < QList<T>::at(i) ) {
      end = i;
    } else {
      st = i;
    }
  }
  return ( end > start ) ? st : -1;
}

template <class T>
int SortableList<T>::findLT( const T &value, int start ) const
{
  // Do a binary search to find the last item < value
  int st = start - 1;
  int end = QList<T>::count();
  while ( end - st > 1 ) {
    int i = ( st + end ) / 2;
    if ( value <= QList<T>::at(i) ) {
      end = i;
    } else {
      st = i;
    }
  }
  return ( end > start ) ? st : -1;
}

template <class T>
int SortableList<T>::findGE( const T &value, int start ) const
{
  // Do a binary search to find the first item >= value
  int st = start - 1;
  int end = QList<T>::count();
  while ( end - st > 1 ) {
    int i = ( st + end ) / 2;
    if ( value <= QList<T>::at(i) ) {
      end = i;
    } else {
      st = i;
    }
  }
  ++st;
  return ( st == QList<T>::count() ) ? -1 : st;
}

template <class T>
int SortableList<T>::findGT( const T &value, int start ) const
{
  // Do a binary search to find the first item > value
  int st = start - 1;
  int end = QList<T>::count();
  while ( end - st > 1 ) {
    int i = ( st + end ) / 2;
    if ( value < QList<T>::at(i) ) {
      end = i;
    } else {
      st = i;
    }
  }
  ++st;
  return ( st == QList<T>::count() ) ? -1 : st;
}

template <class T>
int SortableList<T>::insertSorted( const T &value )
{
  int i = findLE( value );
  if ( i < 0  ||  QList<T>::at(i) != value ) {
    QList<T>::insert( ++i, value );
  }
  return i;
}

template <class T>
int SortableList<T>::removeSorted( const T &value, int start )
{
  int i = findSorted( value, start );
  if ( i >= 0 ) {
    QList<T>::removeAt( i );
  }
  return i;
}

} // namespace KCalCore

#endif
