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
language modeling approach to retrieval. The original query model can be computed based on the original query text (when the parameter "initQuery" is not set,
or set to a null string), or based on a previously saved query model (the model
is given by the parameter "initQuery"). Expanding a saved query model makes
it possible to do iterative feedback. Feedback can be based on true
relevance judgments or any previously returned retrieval results. 
<p>
Two important notes:
<ul>
<li> All the feedback algorithms currently in Lemur assume that all entries in a judgment file are <em> relevant </em> documents, so you must remove all the entries of judged non-relevant documents. However, the judgment status is recorded
in the internal representation of judgments, so that it is possible to distinguish judged relevant documents from judged non-relevant documents in a feedback
algorithm. 
<li> The format of the judgment file, when used for feedback, 
must be of three columns, i.e., with the second column removed so that
each line has a query id, a document id, and a judgment value. This is to be
consistent with the format of a result file. An alternative would be 
to use the original four-column format directly, but, then we would need
to add a parameter to distinguish this four-column format from the
three-column format of a result file. 
 </uL>


<p>
Parameters:
<ol>
<li> <tt>index</tt>: The complete name of the index table-of-content file for the database index. 

<li> <tt>smoothSupportFile</tt>: The name of the smoothing support file (e.g., one generated by GenerateSmoothSupport).

<li> <tt>textQuery</tt>: the original query text stream 

<li> <tt>initQuery</tt>: the file with a saved initial query model. When this
parameter is set to a non-empty string, the model stored in this file will be
used for expansion; otherwise, the original query text is used the initial
query model for expansion.

<li> <tt>feedbackDocuments</tt>: the file of feedback documents to be used for feedback. In the case of pseudo feedback, this can be a result file generated from an initial retrieval process. In the case of relevance feedback, this is usually a 3-column relevance judgment file. Note that this means you can <em>NOT</em> use a TREC-style judgment file
directly; you must remove the second column to convert it to three-column.  

<li> <tt>TRECResultFormat</tt>: whether the feedback document file (given
by <tt>feedbackDocuments</tt>  is of the TREC format (i.e., six-column) or just
a simple three-column format <queryID, docID, score>. Integer value, zero for non-TREC format, and non-zero for TREC format. Default: 1 (i.e., TREC format).
<em> VERY IMPORTANT: For relevance feedback, <tt>TRECResultFormat</tt> should
always be set to 0, since the judgment file is always a simple format</em>.

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
  enum FBMethod {MIX=0, DIV=1, MAR=2, RM1=3, RM2=4};
  /// retrieval model 
  static enum FBMethod fbmethod;

  String expandedQuery;
  String initQuery;
  String feedbackDocuments;
  bool TRECResultFormat;
  void get() {
    // default is MIX divergence model
    fbmethod = (FBMethod) ParamGetInt("queryUpdateMethod",MIX);    
    expandedQuery = ParamGetString("expandedQuery");
    initQuery = ParamGetString("initQuery",""); 
    feedbackDocuments = ParamGetString("feedbackDocuments");
    // default is TREC format
    TRECResultFormat = ParamGetInt("resultFormat",1); 
  }
};

void GetAppParam()
{
  LocalParameter::get();
  RetrievalParameter::get();  
  SimpleKLParameter::get();
}


void updateQueryModel(QueryRep *qr, char *qid, ResultFile &resFile, RetrievalMethod *model, ofstream &os)

{
  IndexedRealVector *res;
  cout << "query : "<< qid << endl;
  SimpleKLQueryModel *qm = (SimpleKLQueryModel *) qr;
  if (resFile.findResult(qid, res)) {
    res->Sort();
    if(LocalParameter::fbmethod == LocalParameter::RM1 || 
       LocalParameter::fbmethod == LocalParameter::RM2)
      res->NormalizeValues();
    PseudoFBDocs *topDoc = new PseudoFBDocs(*res, 
					    RetrievalParameter::fbDocCount);
    model->updateQuery(*qr, *topDoc);
    os << qid;
    qm->save(os);
    delete topDoc;
  } else {
    cerr << "Warning: no feedback documents found for query: "<< qid << endl;
  }
}

/// A query model estimation program

int AppMain(int argc, char *argv[]) {


  Index  *ind;

  try {
    ind = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  } 
  catch (Exception &ex) {
    ex.writeMessage();
    throw Exception("GenerateQueryModel", "Can't open index, check parameter index");
  }
 

  ArrayAccumulator accumulator(ind->docCount());

  ifstream fbdoc(LocalParameter::feedbackDocuments, ios::in);
  if (fbdoc.fail()) {
    throw Exception("AppMain", "can't open the feedback doc file, check parameter value for feedbackDocuments");
  }

  ofstream os(LocalParameter::expandedQuery);
  
  ResultFile resFile(LocalParameter::TRECResultFormat);
  resFile.load(fbdoc, *ind);
  SimpleKLRetMethod *model =  new SimpleKLRetMethod(*ind, SimpleKLParameter::smoothSupportFile, accumulator);
  model->setDocSmoothParam(SimpleKLParameter::docPrm);
  model->setQueryModelParam(SimpleKLParameter::qryPrm);

  // Use either the original query text or the initial query model stored in 
  // LocalParameter::origQuery.
  DocStream *qryStream;
  ifstream *initQIFS;

  bool useOrigQuery = (strlen(LocalParameter::initQuery)==0);
  
  if (useOrigQuery) {
    cerr << "### Expanding the original text query ...\n";
    try {
      qryStream = new BasicDocStream(RetrievalParameter::textQuerySet);
    } catch (Exception &ex) {
       ex.writeMessage(cerr);
       throw Exception("GenerateQueryModel", "Can't open query file, check parameter textQuery");
    }
  } else {
    cerr << "### Expanding the saved initial query ...\n";
    initQIFS = new ifstream(LocalParameter::initQuery);
    if (initQIFS->fail()) {
      throw Exception("GenerateQueryModel", "Can't open initial query file");
    } 
  }
  
  if (useOrigQuery) {
    qryStream->startDocIteration();
    TextQuery *q;
    while (qryStream->hasMore()) {
      Document *d = qryStream->nextDoc();
      q = new TextQuery(*d);
      QueryRep *qr = model->computeQueryRep(*q);
      updateQueryModel(qr, q->id(),  resFile, model, os);     
      delete qr;
      delete q;
    }
  } else {
    char qid[1024];
    while ( *initQIFS >> qid) {
      SimpleKLQueryModel *qm = new SimpleKLQueryModel(*ind);
      qm->load(*initQIFS);
      updateQueryModel(qm, qid,  resFile, model, os);      
    }
  }
  os.close();
  delete model;
  if (useOrigQuery) {
    delete qryStream;
  } else {
    delete initQIFS;
  }

  delete ind;
  return 0;
}


