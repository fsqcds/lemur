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

#ifndef _INVFPTERM_HPP
#define _INVFPTERM_HPP

#include "TermInfoList.hpp"
#include "InvFPTypes.hpp"

/// term class for InvIndex and InvFPIndex
/*! 
*/

class InvFPTerm: public TermInfo {
public:
  friend class InvTermList;
  friend class InvFPTermList;
  friend class InvFPTermPropList;

  InvFPTerm() { freq=1;};
  ~InvFPTerm() {};

  const char* spelling() const { return word; }
  void spelling(const char* term) { word = term; }
  int strLength() const { return strlen; }
  void strLength(int len) { strlen = len; }
  int count() const {return freq; }
  TERMID_T id() const { return tid; }
  LOC_T position() const { return loc; }
  const int* positions() const {vector<LOC_T>::const_iterator it(loclist->begin()); return &(*it); }
  void position(LOC_T pos) { loc = pos; }

protected:

  TERMID_T tid;
  int strlen;
  const char* word; 
  int freq; // number of times this term occurs in this documen
  LOC_T loc;  // where this term (currently) occurs in the document
  vector<LOC_T>* loclist; // list of all places term occurs in the document                  
};

#endif
