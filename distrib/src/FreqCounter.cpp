

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

#include "FreqCounter.hpp"

#include "common_headers.hpp"


// need time to seed the random number generator
#ifdef _WIN32
#include <afx.h>
#else
#include <sys/time.h>
#endif
#include <cctype>



FreqCounter::FreqCounter(Stopper * stopWords) {
  stopper = stopWords;

  // Set defaults
  randomMode = R_CTF;
  srand(time(NULL));
  ctfTot = 0;
  dfTot = 0;
  nWords = 0;
  atfValid = false;
  name = NULL;
}

FreqCounter::FreqCounter(char * filename, Stopper * stopWords) {
  stopper = stopWords;

  // Set defaults
  randomMode = R_CTF;
  srand(time(NULL));
  ctfTot = 0;
  dfTot = 0;
  nWords = 0;
  atfValid = false;
  name = NULL;

  // Load statistics from file
  input(filename);
}

FreqCounter::~FreqCounter() {
  // Free memory
  freqmap::iterator curr = freqInfo.begin();
  while (curr != freqInfo.end()) {
    free (curr->first);
    curr++;
  }
  free (name);
}

void
FreqCounter::clear() {

  // Empty containers
  randdone.clear();
  doc.clear();

  // Free memory
  freqmap::iterator curr = freqInfo.begin();
  while (curr != freqInfo.end()) {
    free (curr->second.word);
    curr++;
  }

  freqInfo.clear();

  // Reset counts
  ctfTot = 0;
  dfTot = 0;
  nWords = 0;
  atfValid = false;
}

char * 
FreqCounter::handleWord(char * word) {
  char * oword = word;
  if (word != NULL) {
    if (stopper == NULL || !stopper->stopWord(word)) {

      freqmap::iterator curr = freqInfo.find(word);
      if (curr == freqInfo.end()) {    
	// Create a new freqinfo_t structure
	word = strdup(word);
	freqinfo_t fi;
	fi.word = word;
	fi.ctf = 0;
	fi.df = 0;
	freqInfo[word] = fi;
	nWords++;
      } else { 
	word = curr->second.word;
      }
      // Increment counts
      freqInfo[word].ctf++;
      ctfTot++;

      // Store this word in the doc set
      doc.insert(word);

      // Average term frequency sum is no longer valid.
      atfValid = false;
    }
  }
  return oword;    
}

char * 
FreqCounter::handleDoc(char * docno) {
  endDoc();
  return docno;
}

void
FreqCounter::endDoc() {
  // Update the document frequencies.

  // Iterate over terms in the document.
  stringset::iterator curr = doc.begin();
  while (curr != doc.end()) {
//      if (freqInfo.find(*curr) == freqInfo.end()) {
//        cerr << "Word not found: " << *curr << endl;
//      }
    // Up the document frequency for the word.
    freqInfo[*curr].df++;
    dfTot += 1;
    curr++;
  }

  // Average term frequency sum is no longer valid.
  atfValid = false;
  
  // Clear the doc set.
  doc.clear();
}


void 
FreqCounter::input(char * filename) {
  char word[5000];
  int ctfc;
  int dfc;

  // Load statistics from the file.
  ifstream istr(filename);
  while (! istr.eof()) {
    // Read in a line from the file.
    istr >> word;
    if (istr.eof()) break;
    istr >> ctfc;
    if (istr.eof()) break;
    istr >> dfc;

    if (stopper == NULL || !stopper->stopWord(word)) {

      // Create a new freqinfo_t structure.
      char * w = strdup(word);
      freqinfo_t fi;
      fi.word = w;
      fi.ctf = ctfc;
      fi.df = dfc;

      // Place structure in the map
      freqInfo[w] = fi;

      // Update totals
      ctfTot += ctfc;
      dfTot += dfc;
      nWords++;
      
    }
  }

  // Average term frequency sum is no longer valid.
  atfValid = false;
  istr.close();
}


// Output counts to file.
void
FreqCounter::output(char * filename) {
  ofstream outfile(filename);
  // Iterate over terms in the freqInfo map
  freqmap::iterator curr = freqInfo.begin();
  while (curr != freqInfo.end()) {
    char * word = curr->first;
    freqinfo_t fi = curr->second;
    // dumb error detection
    if (!(fi.df > fi.ctf || fi.ctf > ctfTot || fi.df > dfTot)) 
      // Output word, collection term frequency, and document frequency.
      outfile << fi.word << "\t" << fi.ctf << "\t" << fi.df << endl;
    else 
      cerr << "Bogus word." << endl;
    curr++;
  }
}


// Select a random word.
char *
FreqCounter::randomWord() {
  char * w;

  // Find a random word given the random word selection mode.
  if (randomMode == R_AVE_TF)
    w = randomAveTf();
  else if (randomMode == R_DF) 
    w = randomDf();
  else if (randomMode == R_UNIFORM) 
    w = randomUniform();
  else 
    w = randomCtf();
 
  // Check to see if this word has been selected before.
  stringset::iterator it = randdone.find(w);
  if (it != randdone.end()) {
    // Word was not unique, select another.
    free (w);
    w = randomWord();
  }

  // Word was unique, return it.
  return w;
}

char *
FreqCounter::randomCtf() {
  // Random number from 1 to collection term frequencies total
  int n = (rand() % ctfTot) + 1;
  int i = 0;
  // Iterate over terms until i (= sum of ctf for words seen so far)
  // is at least n.
  freqmap::iterator curr = freqInfo.begin();
  i += curr->second.ctf;
  while (curr != freqInfo.end() && i < n) {
    curr++;
    i += curr->second.ctf;
  }
  return strdup(curr->first);
}

char *
FreqCounter::randomDf() {
  // Random number from 1 to document frequencies total
  int n = (rand() % dfTot) + 1;
  int i = 0;
  // Iterate over terms until i (= sum of df for words seen so far)
  // is at least n.
  freqmap::iterator curr = freqInfo.begin();
  i += curr->second.df;
  while (curr != freqInfo.end() && i < n) {
    curr++;
    i += curr->second.df;
  }
  return strdup(curr->first);
}

char *
FreqCounter::randomAveTf() {
  if (atfValid == false) {
    // Recompute avetfTot
    avetfTot = 0;
    freqmap::iterator curr = freqInfo.begin();
    while (curr != freqInfo.end()) {
      avetfTot += curr->second.ctf/ (long double) curr->second.df;
      curr++;
    }    
    atfValid = true;
  }


  // Random number between 0 and 1
  long double n = rand() / (long double) LONG_MAX;
  long double i = 0;
  // Iterate over terms until i (= sum of avetf/avetfTot for words seen so far)
  // is at least n.
  freqmap::iterator curr = freqInfo.begin();
  i += (curr->second.ctf / (long double) curr->second.df) / avetfTot;
  while (curr != freqInfo.end() && i < n) {
    curr++;
    i += (curr->second.ctf / (long double) curr->second.df) / avetfTot;
  }
  return strdup(curr->first);
}

char *
FreqCounter::randomUniform() {
  // Random number between 1 and number of unique words
  int n = (rand() % nWords) + 1;
  int i = 0;
  // Iterate over terms until i (= number words seen so far)
  // is at least n.
  freqmap::iterator curr = freqInfo.begin();
  i++; 
  while (curr != freqInfo.end() && i < n) {
    curr++;
    i++;
  }
  return strdup(curr->first);
}

// Compute the collection term frequecy ratio of two LMs.
// This model is the reference model.
double
FreqCounter::ctfRatio(FreqCounter & lm) {
  double ctfSum = 0.0;
  double ctfTot2 = 0.0;
  freqmap::iterator curr = freqInfo.begin();
  while (curr != freqInfo.end()) {
    if (lm.getCtf(curr->first) > 0) {
      ctfSum += curr->second.ctf;
    }
    ctfTot2 += curr->second.ctf;
    curr++;
  }
  return (ctfSum / (double) ctfTot2);
}

// Used by pruneBottomWords - stores a set ordered
// by ctf then word.
struct ltctf
{
  bool operator()(freqinfo_t f1, freqinfo_t f2) const{
    if (f1.ctf == f2.ctf) 
      return strcmp(f2.word, f1.word) < 0;
    return (f1.ctf - f2.ctf) < 0;
  }
};
typedef set<freqinfo_t, ltctf> ctfset;


// Remove all but the most frequent numTopWords words.
void
FreqCounter::pruneBottomWords(int numTopWords) {
  freqmap::iterator curr = freqInfo.begin();
  int nw = 1;
  ctfset topWords;
  freqinfo_t min = curr->second;
  curr++;
  while (curr != freqInfo.end()) {
    freqinfo_t fi = curr->second;
    if (nw < numTopWords) {
      // not enough words in topWords yet, add the next
      topWords.insert(fi);
      if (fi.ctf < min.ctf) {
	min = fi;
      }
      nw++;
    } else {
      if (fi.ctf > min.ctf) {
	topWords.erase(min);
	free(min.word);
	topWords.insert(fi);
	ctfset::iterator tmp = topWords.begin();
	min = *tmp;

      }
    }
    curr++;
  }
  freqInfo.clear();
  ctfset::iterator it = topWords.begin();
  while (it != topWords.end()) {
    freqInfo[(*it).word] = *it;
    it++;
  }

}

//------ Get and set functions below. ------
 
void
FreqCounter::setName(char * freqCounterName) {
  name = strdup(freqCounterName);
}

char *
FreqCounter::getName() {
  return name;
}


freqmap *
FreqCounter::getFreqInfo() {
  return &freqInfo;
}

void 
FreqCounter::setRandomMode(int mode) {
  // only change the mode if it is in the right range 
  if (mode <= R_UNIFORM && mode >= R_CTF) {
    randomMode = mode;
  }
}

int
FreqCounter::getRandomMode() {
  return randomMode;
}

int 
FreqCounter::getCtf(char * word) {
  if (freqInfo.find(word) != freqInfo.end()) {
    freqinfo_t fi = freqInfo[word];
    return fi.ctf;
  }
  return 0;
}

int
FreqCounter::getDf(char * word) {
  if (freqInfo.find(word) != freqInfo.end()) {
    freqinfo_t fi = freqInfo[word];
    return fi.df;
  }
  return 0;
}

// Return the average term frequency for a word
double
FreqCounter::getAveTf(char * word) {
  if (freqInfo.find(word) != freqInfo.end()) {
    freqinfo_t fi = freqInfo[word];
    return (fi.ctf / (double) fi.df);
  }
  return 0;
}


// Return number of unique words.
int 
FreqCounter::numWords() {
  return nWords;
}

// Return total count of words.
int 
FreqCounter::totWords() {
  return ctfTot;
}