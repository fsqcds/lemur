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


#ifndef _INVFPPUSHINDEX_HPP
#define _INVFPPUSHINDEX_HPP

//! Example Class for push method of building an index

/*!

  This class builds an index assigning termids, docids, tracking locations of term within documents, and tracking terms within documents.  It also expects a DocumentProp to have the total number of terms that were in a document.  It expects that all stopping and stemming (if any) occurs before the term is passed in.

*/

/*
 * NAME DATE - COMMENTS
 * tnt 03/01 - created
 * tnt 06/02 - subclass from InvPushIndex
 *
 ======================================================================*/
#include "common_headers.hpp"
#include "MemCache.hpp"
#include "InvFPTypes.hpp"
#include "InvFPDocList.hpp"
#include "InvFPTerm.hpp"
#include "InvFPIndexMerge.hpp"
#include "InvPushIndex.hpp"

class InvFPPushIndex : public InvPushIndex {
public:
  InvFPPushIndex(char* prefix="DefaultIndex", int cachesize=128000000, long maxfilesize=2100000000, DOCID_T startdocid=1);
  ~InvFPPushIndex();

  /// adding a term to the current document, returns true if term was added successfully.  
  bool addTerm(Term& t);

  /// signify the end of current document
  void endDoc(DocumentProps* dp);

  /// signify the end of this collection.  properties passed at the beginning of a collection should be handled by the constructor.
  void endCollection(CollectionProps* cp);


private:
  void writeTOC(int numinv);

  vector<LocatedTerm> termlist; /// list of terms and their locations in this document
};

#endif
