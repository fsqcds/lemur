/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#ifndef _MMRSUMM_HPP
#define _MMRSUMM_HPP

#include <iomanip>
#include "Summarizer.hpp"
#include "Passage.hpp"
#include "MMRPassage.hpp"
#include "InvFPIndex.hpp"
#include <algorithm>
#include <vector>
#include <string>
using std::string;
using std::vector;

static const string EOS("*eos");
static const string TITLE("*title");
static const string PRONOUN("*pronoun");

#define PSG_LEN  15

/*!
  A summarizer implementation that uses MMR to create summaries.  This example shows how a slightly more complex summarizer may look, and how to use the <code>Summarizer</code> abstract with the <code>Passage</code> abstract.  Note that this implementation is paired with an implementation of <code>Passage</code> called <code>MMRPassage</code>.

*/

class MMRSumm : public Summarizer {

private:
  double lambda;
  const InvFPIndex* idx;
  int summLen;
  vector<MMRPassage> doc;
  mutable int iterCount;
  double maxSims;
  MMRPassage* queryPassage;

  int autoMMRQuery(void) {
    TermInfo* tEntry;
    TermInfoList* tList = idx->termInfoListSeq(idx->document(queryPassage->docID));
    termCount* storage;
    if (hasTITLE(idx, tList)) {
      // use title words
      tList->startIteration();
      cout << "title found" << endl;
      while (tList->hasMore()) {
	tEntry = tList->nextEntry();
	if ( isTITLE(idx->term(tEntry->termID())) ) {
	  tEntry = tList->nextEntry(); // the actual word after title token
	  storage = new termCount;
	  storage->termID = tEntry->termID();
	  storage->tf = tEntry->count();
	  storage->val = tEntry->count();
	  queryPassage->addTerm(*storage);
	}
      }      
    } else {
      tList->startIteration();
      for (int i=0; i<10; i++) {
	if (tList->hasMore()) {
	  tEntry = tList->nextEntry();
	  storage = new termCount;
	  storage->termID = tEntry->termID();
	  storage->tf = tEntry->count();
	  storage->val = tEntry->count();
	  queryPassage->addTerm(*storage);
	}
      } 
    }
    cout << "Autoquery: ";
    showPassage((*queryPassage).getAsVector(), idx);
    cout << endl;

    return 1;
  }

  int setMMRQuery(const string &qInfo) {
    if (qInfo != "") {
      termCount* storage;
      storage = new termCount;
      storage->termID = idx->term(qInfo);
      storage->tf = 1;
      storage->val = 1;
      queryPassage->addTerm(*storage);
      return 1;
    }
    return autoMMRQuery();
  }

public:

  MMRSumm(const InvFPIndex* inIdx, int inSummLen = 5) {
    idx = inIdx;
    summLen = inSummLen;
    iterCount = 1;
    maxSims = -1.0;
    queryPassage = NULL;
    lambda = 1.0;
  };
  
  virtual void markPassages(int optLen, const string &qInfo);

  virtual void addPassage(Passage &psg);

  void addDocument(const string &docID);

  virtual int fetchPassages(Passage* psgs, int optLen) const;
  
  virtual void summDocument(const string &docID, const int optLen, const string &qInfo);

  virtual void scorePassages(const string &qInfo);

  virtual void clear(void);

  virtual int nextPassage(Passage* psg) const;

  virtual void iterClear(void) const;

  virtual void outputSumm(void) const;

  void findNextPassage(MMRPassage &psg, const InvFPIndex* idx, 
		       const TermInfoList* tList, int eos);

  void showPassage(const passageVec* psg, const InvFPIndex* idx) const;
  
  void showMarkedPassages() const ;

  int isEOS(const string &check) {
    return (check == EOS);
  }
  
  int hasEOS(const InvFPIndex* idx, const TermInfoList* tList) {
    tList->startIteration();
    TermInfo* tEntry;
    while (tList->hasMore()) {
      tEntry = tList->nextEntry();
      if ( isEOS(idx->term(tEntry->termID())) ) return true;
    }
    return false;
  }
  
  int isTITLE(const string & check) {
    //    return !strcmp(check, TITLE);
    return (check == TITLE);
  }
  
  int hasTITLE(const InvFPIndex* idx, const TermInfoList* tList) {
    tList->startIteration();
    TermInfo* tEntry;
    while (tList->hasMore()) {
      tEntry = tList->nextEntry();
      if ( isTITLE(idx->term(tEntry->termID())) ) return true;
    }
    return false;
  }
  
  int isPRONOUN(const string &check) {
    return (check == PRONOUN);
  }
  
  struct compareSW {
    double lambda;
    compareSW(double l) { lambda = l; }
    bool operator()(const MMRPassage p1, const MMRPassage p2) const {
      return p1.computeMMR(lambda) > p2.computeMMR(lambda);
    }
  };
  
}; // MMRSumm

#endif
