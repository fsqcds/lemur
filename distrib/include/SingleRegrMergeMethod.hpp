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

#ifndef _SINGLEREGRMERGEMETHOD_HPP
#define _SINGLEREGRMERGEMETHOD_HPP

/** 
    Implementation of regression merging method for single database type case
    (All the individual databases are using Inquery(CORI) retrieval algorithm.
    Build a single regression model for all the databases.
    When there is not enough training data, the algorithm backs off to CORI results merging algorithm.
    Assume all the documents scores have been normalized 
    See RetrievalMethod for database ranking algorithms (CORIRetMethod).
    See DistSearchMethod for multiple individual database retrieval.
    See CORIMergeMethod for CORI results merging algorithm
 **/


/// The minimum number of overlap documents (training data points) for a single databases
#define MINOVERLAPDOCNUM 3 
/// The threshold of backing off to CORI results merging algorithm.
#define MINLESSDBPERCEN 0.4
/// Only use the top n documents retrieved from a database to build the regression model.
#define GETTOPDOCSNUM 20

#include "Param.hpp"
#include "DistMergeMethod.hpp"
#include "InvFPTypes.hpp"


namespace SingleRegrMergeMethodParameter {

  //@{ 
  /// central database index
  static String csDbDataBaseIndex;
  //@}

  /// To get the filename of centralized sampling database
  static void get() {
    csDbDataBaseIndex = ParamGetString("csDbDataBaseIndex","");
  }
};


class SingleRegrMergeMethod : public DistMergeMethod {
public:
  /// the default values of constA and constB are the values of CORI merging algorithm
  SingleRegrMergeMethod(double constA=0.714, double constB=0.2857);
  ~SingleRegrMergeMethod();

  /// calculate the single regression model for all the databases
  /// indexset are the database scores for selected databases
  /// centralsocres are the central documents scores retrieved by centralized sampling database
  /// scoresset are the distributed documents scores retrieved by individual databases
  void calcRegrParams(IndexedRealVector &indexset, DocScoreVector* centralscores, DocScoreVector** scoresset);

protected:
  /// create a score for the merge based on the index ranking score and the document score 
  virtual double score(double dbscore, double docscore);

  /// The model parameters.
  double parama;
  double paramb;
};

#endif