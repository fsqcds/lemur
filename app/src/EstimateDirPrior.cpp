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


/// Leave-one-out estimation of Dirichlet prior smoothing parameter for a collection

/*! \page EstimateDirPrior  EstimateDirPrior Application

 

This application (EstimateDirPrior.cpp) uses the leave-one-out method to
estimate an optimal setting for the Dirichlet prior smoothing parameter
(i.e., the "prior sample size").

To run the application, follow the general steps of running a lemur
application and set the following variables in the parameter file:

(1) index: the table-of-content (TOC) record file of the index 
(e.g., the .bsc file created by BuildBasicIndex)

(2) initValue: the initial value for the parameter in the Newton method.

It will print out the estimated parameter value to the standard output.
*/


#include <fstream.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "IndexManager.hpp"
#include "BasicIndex.hpp"
#include "IndexCount.hpp"
#include "Param.hpp"
#include "String.hpp"


namespace LocalParameter {
  static String index;
  double initialMu;
  void get() {
    index = ParamGetString("index");
    initialMu = ParamGetDouble("initValue",1);
  }
};

void GetAppParam()
{
  LocalParameter::get();
}

int AppMain(int argc, char *argv[]) {


  Index * ind = IndexManager::openIndex(LocalParameter::index);

  double func, deriv,LL;
  int d;

  double mu = LocalParameter::initialMu;
  double meanFit = 1e-40;

  int itCount=0;

  cout << endl << "Starting Newton iterations ..." << endl << endl;
  do {
    itCount++;
    func=0;
    deriv=0;
    LL=0;
    for (d=1; d<= ind->docCount(); d++) {
      double dlen = ind->docLength(d);
      TermInfoList *tList = ind->termInfoList(d);
      tList->startIteration();
      while (tList->hasMore()) {
	TermInfo *info = tList->nextEntry();
	double fq = info->count();
	int id = info->id();
	double colPr = (ind->termCount(id)+1)
	  /(double)(ind->termCountUnique()+ind->termCount());
	double tmp1 = fq-1+mu*colPr;
	double tmp2 = dlen-1+mu;
	double tmp3 = colPr*dlen-colPr-fq+1;
      
	func += fq*tmp3/(tmp1*tmp2);
	deriv += (-fq*tmp3*tmp3/(tmp1*tmp1*tmp2*tmp2));
	LL += fq*log(tmp1/tmp2);
      }
      delete tList;
    }
    
    cout << itCount << ": mu="<<mu<<"  log-likelihood="<< LL << " inc="<< func/deriv << endl;
    mu -= func/deriv;
    meanFit = 0.5*meanFit+0.5*LL;
  } while ( (itCount < 50000) &&
	    (fabs(func/deriv)>0.000001) && 
	    (fabs((meanFit-LL)/meanFit)>0.00000001));
  
  cout << endl;
  cout << "####### Final prior_sample_size = " << mu << " with log-likelihood = "<< LL << endl << endl;
  return 0;

}



