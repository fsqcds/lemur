#include "TFIDFRetMethod.hpp"
#include "Param.hpp"
#include <math.h>


TFIDFQueryRep::TFIDFQueryRep(TextQuery &qry, Index &dbIndex, double *idfValue, TFIDFParameter::WeightParam &param): ArrayQueryRep (dbIndex.termCountUnique()+1, qry, dbIndex), ind(dbIndex), idf(idfValue), prm(param)
{
  startIteration();
  while (hasMore()) {
    QueryTerm *qt = nextTerm();
    setCount(qt->id(), queryTFWeight(qt->weight())*idf[qt->id()]);
    cout << "term : "<< dbIndex.term(qt->id()) << " idf="<< idf[qt->id()] << 
      " total "<< dbIndex.docCount() << " with term "<< dbIndex.docCount(qt->id()) << endl;
  }
}
double TFIDFQueryRep::queryTFWeight(const double rawTF)
{
  if (prm.tf == TFIDFParameter::RAWTF) {
    return (rawTF);
  } else if (prm.tf == TFIDFParameter::LOGTF) {
    return (log(rawTF+1));
  } else if (prm.tf == TFIDFParameter::BM25) {
    return (TFIDFRetMethod::BM25TF(rawTF,prm.bm25K1,0,
		   1, 1));  // no length normalization for query 
  } else {  // default to raw TF
    cerr << "Warning: unknown TF method, raw TF assumed\n";
    return rawTF;
  }
}



double TFIDFDocRep::docTFWeight(const double rawTF)
{
  if (prm.tf == TFIDFParameter::RAWTF) {
    return (rawTF);
  } else if (prm.tf == TFIDFParameter::LOGTF) {
    return (log(rawTF+1));
  } else if (prm.tf == TFIDFParameter::BM25) {
    
    return (TFIDFRetMethod::BM25TF(rawTF, prm.bm25K1, prm.bm25B,
		   ind.docLength(id), ind.docLengthAvg()));
  } else {  // default to raw TF
    cerr << "Warning: unknown TF method, raw TF assumed\n";
    return rawTF;
  }
}


TFIDFRetMethod::TFIDFRetMethod(Index &dbIndex)
{
  ind = &dbIndex;
  docTFParam.tf = (TFIDFParameter::TFMethod)ParamGetInt("doc.tfMethod",TFIDFParameter::BM25);
  docTFParam.bm25K1 = ParamGetDouble("doc.bm25K1",1);
  docTFParam.bm25B = ParamGetDouble("doc.bm25B",0.5);
  
  qryTFParam.tf = (TFIDFParameter::TFMethod)ParamGetInt("query.tfMethod",TFIDFParameter::BM25);
  qryTFParam.bm25K1 = ParamGetDouble("query.bm25K1",1);
  qryTFParam.bm25B = 0;
 
  howManyTerms = ParamGetInt("feedbackDocCount",0); // no feedback
  posCoeff = ParamGetDouble("feedbackPosCoeff", 4); 
  // pre-compute IDF values
  idfV = new double[dbIndex.termCountUnique()+1];
  for (int i=1; i<=dbIndex.termCountUnique(); i++) {
    idfV[i] = log((dbIndex.docCount()+1)/(0.5+dbIndex.docCount(i)));
  }
  scFunc = new ScoreFunction();
}




void TFIDFRetMethod::updateQuery(QueryRep &qryRep, DocIDSet &relDocs)
{
  int totalTerm=ind->termCountUnique();  
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

    TermInfoList *tList = ind->termInfoList(docID);
    tList->startIteration();
    while (tList->hasMore()) {
      TermInfo *info = tList->nextEntry();
      TFIDFDocRep *dr = dynamic_cast<TFIDFDocRep *>(computeDocRep(docID));
      centroidVector[info->id()] += dr->docTFWeight(info->count());
      delete dr;
    }
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
    if (termCount++ >= howManyTerms) {
      break;
    } else {
      // add the term to the query vector
      (dynamic_cast<TFIDFQueryRep *>(&qryRep))->incCount((*j).ind, (*j).val*posCoeff);
    }
  }

}


