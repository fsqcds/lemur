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



/// A query model generation program for the KL-divergence retrieval model



/*! \page GenerateQueryModel Query Model Generation Application 

<p>
This application (GenerateQueryModel.cpp) computes an expanded query model based on feedback documents and the original query model for the KL-divergence retrieval method. It can be regarded as performing a feedback in the
language modeling approach to retrieval.
<p>
Parameters:
<ol>
<li> <tt>index</tt>: The complete name of the index table-of-content file for the database index. 

<li> <tt>smoothSupportFile</tt>: The name of the smoothing support file (e.g., one generated by GenerateSmoothSupport).

<li> <tt>textQuerySet</tt>: the original query text stream 

<li> <tt>resultFile</tt>: the result file to be used for feedback

<li> <tt>TRECResultFormat</tt>: whether the result format is of the TREC format (i.e., six-column) or just
a simple three-column format <queryID, docID, score>. Integer value, zero for non-TREC format, and non-zero for TREC format. Default: 1 (i.e., TREC format)

<li> <tt>expandedQuery</tt>: the file to store the expanded query model

<li> <tt>feedbackDocCount</tt>: the number of docs to use for pseudo-feedback (0 means no-feedback)

<li> <tt>queryUpdateMethod</tt>: feedback method (0, 1, 2 for mixture model, divergence minimization, and Markov chain respectively).


<li>  Method-specific feedback parameters:
<p>
For all interpolation-based approaches (i.e., the new query model is an interpolation of the original
model with a (feedback) model computed based on the feedback documents), the following four
parameters apply:
<ol>
<li> <tt>feedbackCoefficient</tt>: the coefficient of the feedback model for interpolation. The value is in [0,1], with 0 meaning using only the original model (thus no updating/feedback) and 1 meaning using only the feedback model (thus ignoring the original model).

<li> <tt>feedbackTermCount</tt>: Truncate the feedback model to no more than a given number of words/terms.

<li> <tt>feedbackProbThresh</tt>: Truncate the feedback model to include only words with a probability higher than this threshold. Default value: 0.001.

<li> <tt>feedbackProbSumThresh</tt>: Truncate the feedback model until the sum of the probability of the included words reaches this threshold. Default value: 1.
<p>
</ol>
Parameters <tt>feedbackTermCount</tt>, <tt>feedbackProbThresh</tt>, and <tt>feedbackProbSumThresh</tt> work conjunctively to control the truncation, i.e., the truncated model must satisfy all the three constraints. 
<p>
All the three feedback methods also recognize the parameter <tt>feedbackMixtureNoise</tt> (default value :0.5), but with <font color=red> <em> different</em> interpretations</font>.  
<ul>
<li> For the collection mixture model method, <tt>feedbackMixtureNoise</tt> is the collection model selection probability in the mixture model. That is, with this probability, a word is picked according to the collection language model, when a feedback document is "generated".
<li> For the divergence minimization method, <tt>feedbackMixtureNoise</tt> means
the weight of the divergence from the collection language model. (The higher it is, the farther the estimated model is from the collection model.)
<li> For the Markov chain method, <tt>feedbackMixtureNoise</tt> is the probability
of <em>not</em> stopping, i.e., <tt>1- alpha</tt>, where alpha is the stopping probability while walking through the chain. 
</ul>
<p>
In addition, the collection mixture model also recognizes the parameter 
<tt>emIterations</tt>, which is the maximum number of iterations the EM algorithm will run. Default: 50. (The EM algorithm can terminate earlier if the log-likelihood converges quickly, where convergence is measured by some hard-coded criterion. See the source code in <tt>SimpleKLRetMethod.cpp</tt> for details. )
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
  String expandedQuery;
  bool TRECResultFormat;
  void get() {
    expandedQuery = ParamGetString("expandedQuery");
    TRECResultFormat = ParamGetInt("resultFormat",1); // default is TREC format
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
  
  Index  *ind = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  DocStream *qryStream = new BasicDocStream(RetrievalParameter::textQuerySet);

  ArrayAccumulator accumulator(ind->docCount());

  ifstream result(RetrievalParameter::resultFile, ios::in);
  if (result.fail()) {
    throw Exception("AppMain", "can't open the result file, check parameter value for resultFile");
  }

  ofstream os(LocalParameter::expandedQuery);
  
  ResultFile resFile(LocalParameter::TRECResultFormat);
  resFile.openForRead(result, *ind);
  SimpleKLRetMethod *model =  new SimpleKLRetMethod(*ind, SimpleKLParameter::smoothSupportFile, accumulator);
  model->setDocSmoothParam(SimpleKLParameter::docPrm);
  model->setQueryModelParam(SimpleKLParameter::qryPrm);

  IndexedRealVector res;

  qryStream->startDocIteration();
  TextQuery *q;
  while (qryStream->hasMore()) {
    Document *d = qryStream->nextDoc();
    q = new TextQuery(*d);
    cout << "query : "<< q->id() << endl;
    QueryRep *qr = model->computeQueryRep(*q);
    resFile.getResult(q->id(), res);
    res.Sort();
    PseudoFBDocs *topDoc = new PseudoFBDocs(res, RetrievalParameter::fbDocCount);
    model->updateQuery(*qr, *topDoc);
    SimpleKLQueryModel *qm = (SimpleKLQueryModel *) qr;
    os << q->id();
    qm->save(os);
    delete qr;
    delete q;
    delete topDoc;
  }

  os.close();
  delete model;
  delete qryStream;
  delete ind;
  return 0;
}


