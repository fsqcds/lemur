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


/// Evaluating a query model using the KL-divergence retrieval function



/*! \page QueryModelEval  Query Model Evaluation Application 
<p>
This application (QueryModelEval.cpp) loads an expanded query model (e.g., one computed by GenerateQueryModel), 
and evaluates it with the KL-divergence retrieval model.
<p>
Parameters:
<ol>
<li> <tt>index</tt>: The complete name of the index table-of-content file for the database index.

<li> <tt>smoothSupportFile</tt>: The name of the smoothing support file (e.g., one generated by GenerateSmoothSupport).

<li> <tt>queryModel</tt>: the file of the query model to be evaluted 

<li> <tt>resultFile</tt>: the result file 

<li> <tt>TRECResultFormat</tt>: whether the result format should be of the TREC format 
 (i.e., six-column) or just a simple three-column format <tt>&lt;queryID, docID, score><tt>. Integer value, zero for non-TREC format, and non-zero for TREC format. Default: 1 (i.e., TREC format)

<li> <tt>resultCount</tt>: the number of documents to return as result for each query

<p>
The following are document model smoothing parameters:

<li> <tt>smoothMethod</tt>: One of the three: Jelinek-Mercer (0), Dirichlet prior (1), and Absolute discounting (2)

<li> <tt>smoothStrategy</tt>: Either <tt>interpolate</tt> (0) or <tt>backoff</tt> (1)

<li> <tt>JelinekMercerLambda</tt>: The collection model weight in the JM interpolation method. Default: 0.5

<li> <tt>DirichletPrior</tt>: The prior parameter in the Dirichlet prior smoothing method. Default: 1000

<li> <tt>discountDelta</tt>: The delta (discounting constant) in the absolute discounting method. Default 0.7.
</ol>
 */


#include "Param.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"
#include "common_headers.hpp" 
#include "SimpleKLRetMethod.hpp"
#include "RetParamManager.hpp"
#include "ResultFile.hpp"
namespace LocalParameter {
  String queryModel;
  bool TRECResultFormat;
  bool useWorkingSet;
  String workSetFile;

  void get() {
    queryModel = ParamGetString("queryModel","");
    TRECResultFormat = ParamGetInt("resultFormat",1); // default is TREC format
    useWorkingSet = ParamGetInt("useWorkingSet", 0); //default is to score the whole collection; otherwise, score a subset
    workSetFile = ParamGetString("workingSetFile",""); // working set file name 
  }
};

void GetAppParam()
{
  LocalParameter::get();
  RetrievalParameter::get();
  SimpleKLParameter::get();
}



/// A query model estimation program
int AppMain(int argc, char *argv[]) {
  

  Index  *ind;

  try {
    ind =  IndexManager::openIndex(RetrievalParameter::databaseIndex);
  } 
  catch (Exception &ex) {
    ex.writeMessage();
    throw Exception("QueryModelEval", "Can't open index, check parameter index");
  }

  ifstream *workSetStr;
  ResultFile *docPool;
  if (LocalParameter::useWorkingSet) {
    workSetStr = new ifstream(LocalParameter::workSetFile, ios::in);
    if (workSetStr->fail()) {
      throw Exception("RetEval", "can't open working set file");
    }
    docPool = new ResultFile(false); // working set is always simple format
    docPool->openForRead(*workSetStr, *ind);
  }

  ifstream qmodel(LocalParameter::queryModel, ios::in);

  ArrayAccumulator accumulator(ind->docCount());

  if (qmodel.fail()) {
    throw Exception("QueryModelEval", "can't open the query model file, check the value for parameter queryModel");
  }
  
  ofstream result(RetrievalParameter::resultFile);

  ResultFile resFile(LocalParameter::TRECResultFormat);

  resFile.openForWrite(result, *ind);

  SimpleKLRetMethod model(*ind, SimpleKLParameter::smoothSupportFile, accumulator);
  
  model.setDocSmoothParam(SimpleKLParameter::docPrm);
  model.setQueryModelParam(SimpleKLParameter::qryPrm);

  IndexedRealVector res;
  char qid[300];

  SimpleKLQueryModel *q;
  IndexedRealVector workSetRes;

  while (qmodel >> qid) {
    cout << "Query "<< qid << endl;
    q = new SimpleKLQueryModel(*ind);
    q->load(qmodel);
    PseudoFBDocs *workSet;
    if (LocalParameter::useWorkingSet) {
      docPool->getResult(qid, workSetRes);
      workSet = new PseudoFBDocs(workSetRes, -1); // -1 means using all docs
      model.scoreDocSet(*q,*workSet,res);
    } else {
      model.scoreCollection(*q, res);
    }

    res.Sort();
    resFile.writeResults(qid, &res, RetrievalParameter::resultCount);
    delete q;
  }
  
  result.close();
  delete ind;
  return 0;
}


