/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
#ifndef _INVPASSAGEPUSHINDEX_HPP
#define _INVPASSAGEPUSHINDEX_HPP

//! Passage push index

/*!
  This class builds a passage index assigning termids, docids, 
tracking locations of term within passages (stopwords not counted). Documents
are segmented into passages of size <tt>passageSize</tt> with an overlap of
<tt>passageSize/2</tt> terms per passage. It expects that all stopping and 
stemming (if any) occurs before the term is passed in.
*/

/*
 * NAME DATE - COMMENTS
 * dmf 07/24 - subclassed to make passage indexer
 ======================================================================*/
#include "InvFPPushIndex.hpp"

class InvPassagePushIndex : public InvFPPushIndex {
public:
  InvPassagePushIndex(const string &prefix, int psgSize=50,
		      int cachesize=128000000, long maxfilesize=2100000000, 
		      DOCID_T startdocid=1);
  virtual ~InvPassagePushIndex();
  /// set the value of passageSize to n.
  void setPassageSize(int n);
  /// the beginning of a new document, returns true if initiation was successful
  virtual bool beginDoc(const DocumentProps* dp);

  /// adding a term to the current document, returns true if term was added successfully.  
  virtual bool addTerm(const Term& t);

  /// signify the end of current document
    virtual void doendDoc(const DocumentProps* dp, int mgrid);
  
private:
  /// size of passages, in terms.
  int passageSize;
  /// passageSize/2 for overlapping passages.
  int passageEnd;  
  /// number of passages so far this document.
  int psgCounter;
  /// the current document
  //  DocumentProps *curDoc;
  /// the current document name
  string curDocName;
};

#endif
