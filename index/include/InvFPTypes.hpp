/* type definitions for objects we will use */
#ifndef _INVFPTYPES_H
#define _INVFPTYPES_H

typedef int   FILEID_T;
typedef int   TERMID_T;
typedef TERMID_T LOC_T;  // for simplifying implementation of memory management, it's helpful for termid and location to be of the same type.
typedef TERMID_T DOCID_T;
typedef float SCORE_T;
typedef char* TERM_T;
typedef char* EXDOCID_T;

// suffixes for filenames
#define INVINDEX  ".invfp"
#define INVLOOKUP  ".invlookup"
#define DTINDEX  ".dt"
#define DTLOOKUP  ".dtlookup"
#define TERMIDMAP  ".tid"
#define DOCIDMAP  ".did"
#define MAINTOC  ".toc"

// name for parameters
#define NUMDOCS_PAR "NUM_DOCS"
#define NUMTERMS_PAR "NUM_TERMS"
#define NUMUTERMS_PAR "NUM_UNIQUE_TERMS"
#define AVEDOCLEN_PAR "AVE_DOCLEN"
#define INVINDEX_PAR  "INV_INDEX"
#define INVLOOKUP_PAR  "INV_LOOKUP"
#define DTINDEX_PAR  "DT_INDEX"
#define DTLOOKUP_PAR  "DT_LOOKUP"
#define TERMIDMAP_PAR  "TERMIDS"
#define DOCIDMAP_PAR  "DOCIDS"
#define NUMDT_PAR  "NUM_DTFILES"
#define NUMINV_PAR  "NUM_INVFILES"

struct LocatedTerm { // pair of term and its location
  TERMID_T term;
  LOC_T loc;
};

struct entry {        // an entry in the lookup table for inverted lists index
  FILEID_T fileid;  // which file the word is in
  long offset;        // what the offset into the file is
  int length;         // the length of the inverted list
};

#endif
