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

#include "Stopper.hpp"
#include "PorterStemmer.hpp"
#include "WebParser.hpp"
#include "TrecParser.hpp"
#include "InvFPTextHandler.hpp"
#include "Param.hpp"

// Local parameters used by the indexer 
namespace LocalParameter {
  int memory;

  // name (minus extension) of the database
  char * index;
  // name of file containing stopwords
  char * stopwords;
  // name of file containing acronyms
  char * acronyms;
  // format of documents (trec or web)
  char * docFormat;
  // whether or not to stem
  char * stemmer;

  void get() {
    // my code uses char *'s while the param utils use
    // strings.  maybe I should convert to strings...

    index = strdup(ParamGetString("index"));
    memory = ParamGetInt("memory", 96000000);
    stopwords = strdup(ParamGetString("stopwords"));
    acronyms = strdup(ParamGetString("acronyms"));
    docFormat = strdup(ParamGetString("docFormat"));
    // convert docFormat to lowercase
    for (char * d = docFormat; *d != '\0'; d++) *d = tolower(*d);

    stemmer = strdup(ParamGetString("stemmer"));
    // convert docFormat to lowercase
    for (char * d = stemmer; *d != '\0'; d++) *d = tolower(*d);
    
  }

  // free the memory allocated in get()
  void freeMem() {
    free(index);
    free(stopwords);
    free(acronyms);
    free(docFormat);
    free(stemmer);
  }
};

// put more usage info in here
void usage(int argc, char ** argv) {
  cerr << "Usage:" << endl
       << argv[0] << " paramfile datfile1 datfile2 ... " << endl
       << endl
       << "PushIndexer builds a database using either the TrecParser or " << endl
       << "WebParser class and InvFPPushIndex.  Summary of parameters:" << endl << endl
       << "\tindex - name of the index (without the .ifp extension)" << endl
       << "\tmemory - memory (in bytes) of InvFPPushIndex (def = 96000000)" << endl
       << "\tstopwords - name of file containing stopword list" << endl
       << "\t            Words in this file should be one per line." << endl
       << "\t            If this parameter is not specified, all words " << endl
       << "\t            are indexed." << endl
       << "\tacronyms - name of file containing acronym list (one word" << endl
       << "\t           per line).  Uppercase words recognized as acronyms" << endl
       << "\t           (eg USA U.S.A. USAs USA's U.S.A.) are left " << endl
       << "\t           uppercase if in the acronym list." << endl
       << "\tdocFormat - \"trec\" for standard TREC formatted documents "<< endl
       << "\t            web for web TREC formatted documents (def = trec)" << endl
       << "\tstemmer - \"porter\" to use Porter's stemmer. " << endl
       << "\t          (def = no stemmer)" << endl;
}

// get application parameters
void GetAppParam() {
  LocalParameter::get();
}


int AppMain(int argc, char ** argv) {
  if (argc < 3) {
    usage(argc, argv);
    return -1;
  }

  // Create the appropriate parser.
  Parser * parser;
  if (!strcmp(LocalParameter::docFormat, "web")) {
    parser = new WebParser;
  } else {
    parser = new TrecParser;
  }

  // Create the stopper if needed.
  Stopper * stopper = NULL;
  if (strcmp(LocalParameter::stopwords, "")) {
    stopper = new Stopper(LocalParameter::stopwords);
  }

  // Create the acronym list and tell parser if needed.
  WordSet * acros = NULL;
  if (strcmp(LocalParameter::acronyms, "")) {
    acros = new WordSet(LocalParameter::acronyms);
    parser->setAcroList(acros);
  }
  
  // Create the stemmer if needed.
  Stemmer * stemmer = NULL;
  if (!strcmp(LocalParameter::stemmer, "porter")) {
    stemmer = new PorterStemmer();
  }

  // Create the indexer. (Note: this has an InvFPPushIndex that 
  // it uses to do the indexing, but InvFPTextHandler implements the
  // TextHandler class, so that it is compatible with my parser
  // architecture.  See the TextHandler and InvFPTextHandler classes
  // for more info.)
  InvFPTextHandler indexer(LocalParameter::index, LocalParameter::memory);


  // chain the parser/stopper/stemmer/indexer

  TextHandler * th = parser;

  if (stopper != NULL) {
    th->setTextHandler(stopper);
    th = stopper;
  }

  if (stemmer != NULL) {
    th->setTextHandler(stemmer);
    th = stemmer;
  } 

  th->setTextHandler(&indexer);

  // parse the data files
  for (int i = 2; i < argc; i++) {
    cerr << "Parsing " << argv[i] << endl;
    parser->parse(argv[i]);
  }

  // free memory
  if (acros != NULL) delete acros;
  if (stopper != NULL) delete stopper;
  delete parser;
  LocalParameter::freeMem();
  return 0;
}
