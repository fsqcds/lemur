/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// Extent
//
// 11 February 2004 -- tds
//

#ifndef INDRI_EXTENT_HPP
#define INDRI_EXTENT_HPP

struct Extent {
  int begin;
  int end;

  Extent() {}
  Extent( int b, int e ) : begin(b), end(e) {}
    
  bool contains( const Extent& other ) const {
    return begin <= other.begin && end >= other.end;
  }

  bool before( const Extent& other ) const {
    return end <= other.begin;
  }

  bool beginsBefore( const Extent& other ) const {
    return begin <= other.begin;
  }

  struct begins_before_less {
    bool operator() ( const Extent& one, const Extent& two ) const {
      return one.beginsBefore( two );
    }
  };
};

#endif // INDRI_EXTENT_HPP
