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

#include "LemurDBManager.hpp"

#include "IndexManager.hpp"
#include "StringQuery.hpp"

#include "Param.hpp"
#include "RetParamManager.hpp"

// Open the database.
void
LemurDBManager::open(char * dbname) {
  // Load the parameter file 
  String dbparam(dbname);
  ParamPushFile(dbparam);
  LemurParameter::get();

  // Create the index
  index = IndexManager::openIndex(LemurParameter::dbname);
  // Create the parser
  parser = new LemurMemParser(index);
  // Create the results list
  accumulator = new ArrayAccumulator(index->docCount());
  results = new IndexedRealVector(index->docCount());
  
  // Create the retrieval method
  switch (LemurParameter::mod) {

  case LemurParameter::TFIDF: 
    model = new TFIDFRetMethod(*index, *accumulator);
    TFIDFParameter::get();
    ((TFIDFRetMethod *)model)->setDocTFParam(TFIDFParameter::docTFPrm);
    ((TFIDFRetMethod *)model)->setQueryTFParam(TFIDFParameter::qryTFPrm);
    ((TFIDFRetMethod *)model)->setFeedbackParam(TFIDFParameter::fbPrm);
    break;
  case LemurParameter::OKAPI:
    model = new OkapiRetMethod(*index, *accumulator);
    OkapiParameter::get();
    ((OkapiRetMethod *)model)->setTFParam(OkapiParameter::tfPrm);
    ((OkapiRetMethod *)model)->setFeedbackParam(OkapiParameter::fbPrm);
    break;
  case LemurParameter::KL:
    SimpleKLParameter::get();
    model = new SimpleKLRetMethod(*index, 
				  ParamGetString("smoothSupportFile"), 
				  *accumulator);
    ((SimpleKLRetMethod *)model)->setDocSmoothParam(SimpleKLParameter::docPrm);
    ((SimpleKLRetMethod *)model)->setQueryModelParam(SimpleKLParameter::qryPrm);
    break;
//    case LemurParameter::CORI:
//      CORIParameter::get();
//      model = new CORIRetMethod(*index, *accumulator,
//                                CORIParameter::collectionCounts);
//      break;
  default:
    throw Exception("RetrievalExp", "unknown retModel parameter");
    break;
    
  }

  // Why did I comment this out?
  //  ParamPopFile();

  outfile = NULL;
}

// Return the parser.
MemParser *
LemurDBManager::getParser() {
  return parser;
}


// Query the database.
results_t *
LemurDBManager::query(char * query, int numdocs) {
  // Create the return value container.
  results_t * res = new results_t;
  res->ids = new docid_t[numdocs];

  // Create the query object.
  StringQuery qry(NULL, query);

  // Create the query representation.
  QueryRep * qryRep = model->computeQueryRep(qry); 
  // Retrieve results.

  model->scoreCollection(*qryRep, *results);

  results->Sort();


  IndexedRealVector::iterator j;
  j = results->begin();
  int i;
  for (i = 0; (i < numdocs) && (j != results->end()); i++) {
    // Place document id in the return value.
    res->ids[i] = strdup(index->document(j->ind));    
    j++;
  }
  res->numDocs = i;

  // Free the query representation.
  delete qryRep;

  // Return the results.
  return res;
}

// Return a document given its id.  The doc part of the return value
// structure is really an array of integer term ids.
doc_t *
LemurDBManager::getDoc(docid_t docid) {
  int lemurID = index->document(docid);

  // Create the return value.
  doc_t * doc = new doc_t;
  doc->id = strdup(docid);
 
  // Create thte doc contents part.
  int intlen = index->docLength(lemurID);
  int sizeratio = sizeof(int) / sizeof(char);
  int bytelen = intlen * sizeratio + 1;
  doc->doc = new char[bytelen];
  int * intdoc = (int *) doc->doc;
  int * loc = intdoc;
  bytelen = 0;

  // Write the term ids to the doc.
  TermInfoList * til = index->termInfoList(lemurID);
  while (til->hasMore()) {
    TermInfo * ti = til->nextEntry();
    for (int i = 0; i < ti->count(); i++) {
      *loc = ti->id();
      loc++;
      bytelen += sizeratio; 
    }
  }
  delete til;

  doc->len = bytelen;

  return doc;
}

void
LemurDBManager::setOutputFile(char * filename) {
  if (outfile != NULL) {
    outfile->close();
    delete outfile;
  }
  outfile = new ofstream(filename);
}

// Write a doc to the end of the file
void
LemurDBManager::output(docid_t docid) {
  int c = 0;

  int lemurID = index->document(docid);
  
  *outfile << "<DOC>\n<DOCNO> " << docid << " </DOCNO>\n<TEXT>\n";
  // Write the term ids to the file.
  TermInfoList * til = index->termInfoList(lemurID);
  while (til->hasMore()) {
    TermInfo * ti = til->nextEntry();
    for (int i = 0; i < ti->count(); i++) {
      if (c % 10 == 0) *outfile << "\n";
      *outfile << index->term(ti->id()) << " ";
      c++;
   }
  }
  delete til;

  *outfile << "\n</TEXT>\n</DOC>\n";
}



// Close the database and free memory.
void
LemurDBManager::close() {
  delete index;
  delete model;
  delete accumulator;
  delete results;
  delete parser;
  outfile->close();
  delete outfile;
}