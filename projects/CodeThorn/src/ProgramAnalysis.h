/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef PROGRAMANALYSIS_H
#define PROGRAMANALYSIS_H

#include "Labeler.h"
#include "CFAnalyzer.h"
#include "WorkListSeq.h"
#include "CollectionOperators.h"
#include "PATransferFunctions.h"
#include <set>
#include <string>
#include "PASolver1.h"
#include "DFAstAttribute.h"

namespace CodeThorn {

  using std::set;
  using std::vector;
  using std::string;

#include "PropertyState.h"

class ProgramAnalysis {
 public:
  ProgramAnalysis();
  void setExtremalLabels(set<Label> extremalLabels);
  void initialize(SgProject*);
  virtual void initializeGlobalVariables(SgProject* root);
  virtual void initializeTransferFunctions();
  virtual void initializeSolver()=0;
  void determineExtremalLabels(SgNode*);
  void run();
  PropertyState* createPropertyState();

  // results are accessible through begin/end and iterator.
  typedef vector<Lattice*> AnalyzerData;
  typedef vector<Lattice*> ResultAccess;
  ResultAccess& getResultAccess();
#if 0
  void attachResultsToAst(string);
#endif
  Labeler* getLabeler();
  CFAnalyzer* getCFAnalyzer();
  VariableIdMapping* getVariableIdMapping();
  Flow* getFlow() { return &_flow; }
  Lattice* getPreInfo(Label lab);
  Lattice* getPostInfo(Label lab);
  void attachInInfoToAst(string attributeName);
  void attachOutInfoToAst(string attributeName);

  void attachInfoToAst(string attributeName,bool inInfo);
 protected:
  virtual void transfer(Label label, Lattice& element);
  virtual void solve();
  VariableIdMapping _variableIdMapping;
  Labeler* _labeler;
  CFAnalyzer* _cfanalyzer;
  set<Label> _extremalLabels;
  Flow _flow;
  // following members are initialized by function initialize()
  long _numberOfLabels; 
  vector<Lattice*> _analyzerDataPreInfo;
  vector<Lattice*> _analyzerDataPostInfo;
  WorkListSeq<Label> _workList;
  void setInitialElementFactory(PropertyStateFactory*);

  //typedef AnalyzerData::iterator iterator;
  typedef AnalyzerData::iterator iterator;
#if 0
  iterator begin();
  iterator end();
  size_t size();
#endif

 protected:
  virtual DFAstAttribute* createDFAstAttribute(Lattice*);
  bool _preInfoIsValid;
  void computeAllPreInfo();
  PATransferFunctions* _transferFunctions;
  Lattice* _initialElementFactory;
  PASolver1* _solver;
 private:
  void computePreInfo(Label lab,Lattice& info);
};

} // end of namespace

#endif
