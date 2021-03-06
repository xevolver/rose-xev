#include "FIPointerAnalysis.h"
#include "AnalysisAbstractionLayer.h"

SPRAY::FIPointerAnalysis::FIPointerAnalysis(VariableIdMapping* vim, SgProject* project):_variableIdMapping(vim),_astRoot(project) {
}

SPRAY::FIPointerAnalysis::~FIPointerAnalysis() {
  delete _fipi;
}
void SPRAY::FIPointerAnalysis::run() {
  initialize();
  _fipi->collectInfo();
  _modVariableIds=_fipi->getMemModByPointer();
}

void SPRAY::FIPointerAnalysis::initialize() {
  VariableIdSet usedVarsInProgram = AnalysisAbstractionLayer::usedVariablesInsideFunctions(_astRoot, _variableIdMapping);
  _fipi=new FlowInsensitivePointerInfo(_astRoot, *_variableIdMapping, usedVarsInProgram);
  //cout<<"DEBUG: _modVariableIds.size(): "<<_modVariableIds.size()<<endl;
}

SPRAY::VariableIdSet SPRAY::FIPointerAnalysis::getModByPointer() {
  return _modVariableIds;
}
