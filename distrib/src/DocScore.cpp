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

#include "DocScore.hpp"

DocScoreVector::ScoreAscending DocScoreVector::ascendOrder;
DocScoreVector::ScoreDescending DocScoreVector::descendOrder;

void DocScoreVector::Sort(bool descending)
{
  if (descending) {
    sort(this->begin(), this->end(), descendOrder);
  } else {
    sort(this->begin(), this->end(), ascendOrder);
  }
}


void DocScoreVector::PushValue(const char* str, double value)
{
  DocScore entry;
  entry.id = strdup(str);
  entry.val = value;
  // the vector will make a shallow copy
  push_back(entry);
}

DocScoreVector::~DocScoreVector() {
  //free memory from our strdup in PushValue
  // parent destructor should take care of rest
  for (int i=0;i<size();i++) {
    if ((*this)[i].id) 
      free((*this)[i].id);
  }
}


