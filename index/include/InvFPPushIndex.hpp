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
 ======================================================================*/
#include <iostream>
#include <fstream>
#include "PushIndex.hpp"
#include "MemCache.hpp"
#include "InvFPTypes.hpp"
#include "InvFPDocList.hpp"
#include "InvFPTerm.hpp"
#include "InvFPIndexMerge.hpp"
#include "map.hpp"
#include "vector.hpp"

typedef map<char*, InvFPDocList*, ltstr> TABLE_T;

class InvFPPushIndex : public PushIndex {
public:
  InvFPPushIndex(char* prefix="DefaultIndex", int cachesize=128000000, long maxfilesize=2100000000, DOCID_T startdocid=1);
  ~InvFPPushIndex();

  /// sets the name for this index. the name will be the prefix for all files related to this index
  void setName(char* prefix);

  /// the beginning of a new document, returns true if initiation was successful
  bool beginDoc(DocumentProps* dp);

  /// adding a term to the current document, returns true if term was added successfully.  
  bool addTerm(Term& t);

  /// signify the end of current document
  void endDoc(DocumentProps* dp);

  /// signify the end of this collection.  properties passed at the beginning of a collection should be handled by the constructor.
  void endCollection(CollectionProps* cp);


private:
  void writeTOC(int numinv);
  void writeDocIDs();
  void writeDTIDs();
  void writeCache();
  void lastWriteCache();

  long maxfile; /// the biggest our file size can be
  MemCache* cache; /// the main memory handler for building
 // FILE* writetlist; /// filestream for writing the list of located terms for each document
  ofstream writetlist;
  FILE* writetlookup; /// filestream for writing the lookup table to the docterm db
  vector<LocatedTerm> termlist; /// list of terms and their locations in this document
  vector<char*> docIDs; /// list of external docids in internal docid order
  vector<char*> termIDs; /// list of terms in termid order
  vector<char*> tempfiles; /// list of tempfiles we've written to flush cache
  vector<char*> dtfiles; /// list of dt index files
  
  int tcount;    /// count of total terms
  int tidcount ; /// count of unique terms
  int dtidcount; /// count of unique terms in a current doc
  char* name;    /// the prefix name
  int namelen;   /// the length of the name (avoid many calls to strlen)
  TABLE_T wordtable; /// table of all terms and their doclists

  int* membuf; /// memory to use for cache and buffers
  int membufsize;  // how much memory we have
};

#endif
