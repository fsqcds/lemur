/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#ifndef _INVTERMLIST_HPP
#define _INVTERMLIST_HPP

#include "common_headers.hpp"
#include "InvFPTerm.hpp"
#include "InvFPTypes.hpp"


/*! 

  This is a class that represents the term entries in doc index, to be used with InvFPIndex
  i.e., the ID and frequency of all terms in a document.

*/

class InvTermList : public TermInfoList {
public:
  InvTermList();
  ~InvTermList();

  /// prepare iteration
  void startIteration();

  /// has more entries
  bool hasMore();

  /// Get a pointer to the next entry (pointer to a local static memory)
  TermInfo *nextEntry();

  /// Get the length of this document
  int docLength(){ return length; }

  /// Get the number of terms in this document
  int termCount() { return listlen/2; }

  /// Get the docid this list is for
  int docID() { return uid; }

  /// Read in a TermInfoList object from a file
  /// Assumes the following format: DOCID DOCLENGTH TID TERM_COUNT ..
  bool binRead(ifstream& infile);

private:

  DOCID_T uid; // this doc's id
  int length;  // length of this document (terms + stopwords)
  int* list; // beginning of list of terms and counts
  int* end; // end of list
  int listlen; // number of terms in list
  int* iter;   // index for iterator
};




#endif
