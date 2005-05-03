/*==========================================================================
 * Copyright (c) 2001 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


#include "ArabicStemmer.hpp"

#include <cctype>
#include "common_headers.hpp"

extern char *arabic_stemdir;
extern void *set_stemmer(char *);
extern void freeWordSets();

lemur::parse::ArabicStemmer::ArabicStemmer(const string &stemDir, const string &stemmer) {
  arabic_stemdir = new char[stemDir.length() + 1];
  stemfunc = stemmer;
  strcpy(arabic_stemdir, stemDir.c_str());
  stem_fct = (void (*)(char *, char *)) set_stemmer((char *)stemmer.c_str());
  // use the same identifier as we do for application stemmer parameter
  //  identifier = "arabic";
}

lemur::parse::ArabicStemmer::~ArabicStemmer() {
  freeWordSets();
  delete[](arabic_stemdir);
}  

char * lemur::parse::ArabicStemmer::stemWord(char * word) {
  static char buffer[100];
  stem_fct(word, buffer);
  if (buffer[0])
    return buffer;
  else
    return NULL;
}

void lemur::parse::ArabicStemmer::writePropertyList(lemur::parse::PropertyList* list) const {
  lemur::api::TextHandler::writePropertyList(list);
  Property p("arabicStemFunc");
  p.setValue(stemfunc);
  list->setProperty(&p);
  p.setName("arabicStemDir");
  p.setValue(arabic_stemdir);
  list->setProperty(&p);
}
