/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include <cctype>
#include "common_headers.hpp"
#include "Exception.hpp"
#include "KStemmer.hpp"
// the Krovetz stemmer is in C
extern char *kstem_stemmer(char *);
extern char *stemdir;
extern int read_dict_info();
extern void kstem_release_memory();

lemur::parse::KStemmer::KStemmer(string &datadir) {
  stemdir = new char[datadir.length() + 1];
  strcpy(stemdir, datadir.c_str());
  if (read_dict_info()) {
    //    cerr << "Failure reading kstemmer data files. Check KstemmerDir = " <<      datadir << " parameter." << endl;
    LEMUR_THROW( LEMUR_IO_ERROR, "Error opening stemmer datafiles");
  }
  iden=identifier;
}

lemur::parse::KStemmer::~KStemmer() {
  delete[](stemdir);
  kstem_release_memory();
}

char * lemur::parse::KStemmer::stemWord(char * word) {
  // only stem words that begin with a lowercase letter 
  // (don't stem acronyms or names)
  if (islower(*word)) {
    return kstem_stemmer(word);
  }
  return word;
}

void lemur::parse::KStemmer::writePropertyList(PropertyList* list) const {
  TextHandler::writePropertyList(list);
  Property p("KstemmerDir");
  p.setValue(stemdir);
  list->setProperty(&p);
}
