#include "OkapiRetMethod.hpp"
#include "Param.hpp"


// initial query constructor, no feedback docs assumed
OkapiQueryRep::OkapiQueryRep(TextQuery &qry, Index &dbIndex, double paramK3): ArrayQueryRep(dbIndex.termCountUnique()+1, qry, dbIndex), k3(paramK3) {
  pEst = new int[dbIndex.termCountUnique()+1];
  for (int i=0; i<=dbIndex.termCountUnique(); i++) {
    pEst[i] = 0;
  }
  pNorm = 0; 
}



double OkapiScoreFunc::matchedTermWeight(QueryTerm *qTerm, 
					 QueryRep *qRep, 
					 DocInfo *info, 
					 DocumentRep *dRep)
{
  OkapiQueryTerm * qt = (OkapiQueryTerm *)qTerm;
    //dynamic_cast<OkapiQueryTerm *>qTerm;

  OkapiQueryRep *qr = (OkapiQueryRep *)qRep;
  
  // (dynamic_cast<OkapiQueryRep *>qRep);
  return (OkapiRetMethod::RSJWeight(qt->pEstCount(), qr->pNormCount(), 
		    ind.docCount(qt->id()),
		    ind.docCount()) * 
	  dRep->termWeight(qt->id(),info) * 
	  qt->weight()); 
}


OkapiRetMethod::OkapiRetMethod(Index &dbIndex)
{
  ind = &dbIndex;
  param.k1 = ParamGetDouble("BM25K1",1.2);
  param.b =  ParamGetDouble("BM25B",0.75);
  param.k3 = ParamGetDouble("BM25K3", 7);
  param.expQTF = ParamGetDouble("BM25QTF", 0.5);
  expTermCount = ParamGetInt("feedbackTermCount",20);
  scFunc = new OkapiScoreFunc(dbIndex);
}




void OkapiRetMethod::updateQuery(QueryRep &origRep, DocIDSet &relDocs)
{
  
  int totalTerm=ind->termCountUnique();  
  static int * relCounts = new int[totalTerm+1];


  int i;
  for (i=1;i<=totalTerm;i++) {
    relCounts[i]=0;
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
      relCounts[info->id()]++;
    }
    delete tList;
  }

  IndexedRealVector weightedTerms(0);
  
  // adjust term weight for term selection
  for (i=1; i<= totalTerm; i++) {
    if (relCounts[i] >0 ) {
      weightedTerms.PushValue(i, relCounts[i]*
			      RSJWeight(relCounts[i],actualDocs, 
					ind->docCount(i),
					ind->docCount()));
    }
  }

  OkapiQueryRep *qr =  dynamic_cast<OkapiQueryRep *> (&origRep);
  qr->setPNormCount(actualDocs);
  weightedTerms.Sort();
  IndexedRealVector::iterator j;
  int termCount=0;
  for (j= weightedTerms.begin();j!=weightedTerms.end();j++) {
    if (termCount++ >= expTermCount) {
      break;
    } else {
      // add the term to the query representation
      qr->incPEst((*j).ind, relCounts[(*j).ind]);
      qr->incCount((*j).ind, param.expQTF);
  
      
    } 
  }
}










