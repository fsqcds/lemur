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
/*
  Author: dmf
 */

#include "CosSimRetMethod.hpp"
#include "Param.hpp"
#include <cmath>

/// Construct a query rep from a text query.
CosSimQueryRep::CosSimQueryRep(TextQuery &qry, Index &dbIndex, 
			       double *idfValue): 
  ArrayQueryRep (dbIndex.termCountUnique()+1, qry, dbIndex), 
  ind(dbIndex), idf(idfValue) {
  double qNorm = 0, tmpval;

  startIteration();
  while (hasMore()) {
    QueryTerm *qt = nextTerm();
    tmpval = qt->weight()*idf[qt->id()];
    setCount(qt->id(), tmpval);
    qNorm += tmpval * tmpval;
    delete qt;
  }
  qNorm = sqrt(qNorm);
  startIteration();
  while (hasMore()) {
    QueryTerm *qt = nextTerm();
    setCount(qt->id(), qt->weight()/qNorm);
    delete qt;
  }
}

/// Construct a query rep from an existing document (given document id).
CosSimQueryRep::CosSimQueryRep(int docId, Index &dbIndex, 
			       double *idfValue): 
  ArrayQueryRep (dbIndex.termCountUnique() + 1), 
  ind(dbIndex), idf(idfValue) {
  double qNorm = 0;
  double tmpval;
  int cnt;
  int id;
  TermInfoList *tList = dbIndex.termInfoList(docId);
  TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    id = info->id();
    cnt = info->count();
    tmpval = cnt * idf[id];
    setCount(id, tmpval);
    qNorm += tmpval * tmpval;
  }
  delete tList;
  qNorm = sqrt(qNorm);
  startIteration();
  while (hasMore()) {
    QueryTerm *qt = nextTerm();
    setCount(qt->id(), qt->weight()/qNorm);
    delete qt;
  }
}

CosSimRetMethod::CosSimRetMethod(Index &dbIndex, 
				 ScoreAccumulator &accumulator) :
  TextQueryRetMethod(dbIndex, accumulator) {
  fbParam.howManyTerms = CosSimParameter::defaultHowManyTerms;
  fbParam.posCoeff = CosSimParameter::defaultPosCoeff;
  L2FileName = CosSimParameter::defaultL2File;
  // pre-compute IDF values
  idfV = new double[dbIndex.termCountUnique()+1];
  for (int i=1; i<=dbIndex.termCountUnique(); i++) {
    idfV[i] = log((dbIndex.docCount()+1)/(0.5+dbIndex.docCount(i)));
  }
  loadDocNorms();  
  scFunc = new CosSimScoreFunc(dbIndex);
}

CosSimRetMethod::CosSimRetMethod(Index &dbIndex, const char *L2file,
				 ScoreAccumulator &accumulator) :
  TextQueryRetMethod(dbIndex, accumulator), L2FileName(L2file) {
  fbParam.howManyTerms = CosSimParameter::defaultHowManyTerms;
  fbParam.posCoeff = CosSimParameter::defaultPosCoeff;

  // pre-compute IDF values
  idfV = new double[dbIndex.termCountUnique()+1];
  for (int i=1; i<=dbIndex.termCountUnique(); i++) {
    idfV[i] = log((dbIndex.docCount()+1)/(0.5+dbIndex.docCount(i)));
  }
  loadDocNorms();  
  scFunc = new CosSimScoreFunc(dbIndex);
}

void CosSimRetMethod::loadDocNorms() {
  int  numDocs = ind.docCount();
  docNorms = new double[numDocs + 1];
  for (int j = 0; j <= numDocs; j++) {
    docNorms[j] = 0;
  }
  ifstream ifs;
  ifs.open(L2FileName);
  if (ifs.fail()) {
    cerr << "open file, wrong name:" << L2FileName << endl; 
    return;
  }
  for (int i = 1; i <= numDocs; i++) {
    int id;
    double norm;
    ifs >> id >> norm;
    if (id != i) {
      cerr << "alignment error in support file, wrong id:" << id << endl; 
      exit(1);
    }
    if (norm == 0) norm = 1;
    docNorms[i] = norm;
  }
  ifs.close();
}

CosSimRetMethod::~CosSimRetMethod() {
  delete[](idfV); 
  delete[](docNorms);  
  delete(scFunc);
}

double CosSimRetMethod::docNorm(int docID) {
  // cache norms for docs already seen once.
  if (docNorms[docID] == 0) {
    TermInfoList *qList = ind.termInfoList(docID);
    TermInfo *qInfo;
    qList->startIteration();
    int idx, dtf;
    double norm = 0, tmp;
    while (qList->hasMore()) {
      qInfo = qList->nextEntry();
      idx = qInfo->id();
      dtf = qInfo->count();
      tmp = dtf * idfV[idx];
      norm += tmp * tmp;
    }
    delete qList;
    norm = sqrt(norm);
    if (norm == 0) norm = 1;
    docNorms[docID] = norm;    
  } 
  return docNorms[docID];
}

/// Use same as TFIDFRetMethod
void CosSimRetMethod::updateTextQuery(TextQueryRep &qryRep, DocIDSet &relDocs)
{
  int totalTerm=ind.termCountUnique();  
  static float * centroidVector = new float[totalTerm+1]; // one extra for OOV

  int i;
  for (i=1;i<=totalTerm;i++) {
    centroidVector[i]=0;
  }

  int actualDocs=0;
  relDocs.startIteration();
  while (relDocs.hasMore()) {
    int docID;
    double relPr;
    relDocs.nextIDInfo(docID, relPr);
    actualDocs++;

    TermInfoList *tList = ind.termInfoList(docID);
    tList->startIteration();
    while (tList->hasMore()) {
      TermInfo *info = tList->nextEntry();
      centroidVector[info->id()] += info->count(); // raw TF
    }
    delete tList;
  }

  // adjust term weight to obtain true Rocchio weight
  for (i=1; i<= totalTerm; i++) {
    if (centroidVector[i] >0 ) {
      centroidVector[i] *= idfV[i]/actualDocs;
    }
  }

  IndexedRealVector centVector(0);
  
  for (i=1; i< totalTerm; i++) {
    if (centroidVector[i]>0) {
      IndexedReal entry;
      entry.ind = i;
      entry.val = centroidVector[i];
      centVector.push_back(entry);
    }
  }
  centVector.Sort();
  IndexedRealVector::iterator j;
  int termCount=0;
  for (j= centVector.begin();j!=centVector.end();j++) {
    if (termCount++ >= fbParam.howManyTerms) {
      break;
    } else {
      // add the term to the query vector
      (dynamic_cast<CosSimQueryRep *>(&qryRep))->incCount((*j).ind, (*j).val*fbParam.posCoeff);
    }
  }

}



