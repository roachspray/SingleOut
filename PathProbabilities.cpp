#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Support/BranchProbability.h"
#include "llvm/IR/CFG.h"
#include "llvm/ADT/Hashing.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <map>
#include <set>
#include <vector>

using namespace llvm;

#include "PathProbabilities.h"

void
PathProbabilities::getAnalysisUsage(AnalysisUsage &AU) const
{
	AU.setPreservesCFG();
	AU.addRequired<BranchProbabilityInfoWrapperPass>();
}

class BranchConnect {
	BasicBlock *source;
	BasicBlock *destination;
	int64_t Num;
	int64_t Dom;

public:
	BranchConnect(BasicBlock *B, BasicBlock *BB, int64_t N, int64_t D) 
	  : source(B), destination(BB), Num(N), Dom(D) {}

	BasicBlock *getSourceBlock() {
		return source;
	}
	Function *getSourceFunction() {
		return source->getParent();
	}
	BasicBlock *getDestinationBlock() {
		return destination;
	}
	Function *getDestinationFunction() {
		return destination->getParent();
	}
	double getProbability() {
		return (double)Num/double(Dom);
	}
};

static void
printFunctionName(Function *F)
{
	if (!F->hasName()) {
		errs() << "Function has no name,.\n";
		return;
	}
	errs() << "Function name: " << F->getName().str() << "\n";
	return;
}

bool
PathProbabilities::runOnModule(Module &M)
{
	std::vector<BranchConnect *> rawProbList;
	std::vector<BranchConnect *> probList;

	for (Function &F :  M) {
		if (F.isDeclaration()) {
			continue;
		}

		BranchProbabilityInfo &bpi =   \
		  getAnalysis<BranchProbabilityInfoWrapperPass>(F).getBPI();

		for (BasicBlock &B : F) {
			TerminatorInst *ti = B.getTerminator();
			for (unsigned ii = 0; ii < ti->getNumSuccessors(); ii++) {
				BranchProbability bp = bpi.getEdgeProbability(&B, ii);
				bp.dump();
				BranchConnect *bConn = new BranchConnect(&B,
				  ti->getSuccessor(ii), bp.getNumerator(),
				  bp.getDenominator());
				rawProbList.push_back(bConn);
			}
		errs() << "----------------------------------------\n";
			for (auto it = succ_begin(&B); it != succ_end(&B); ++it) {
				BasicBlock *dst = *it;
				BranchProbability bp = bpi.getEdgeProbability(&B, dst);
				bp.dump();
				BranchConnect *bConn = new BranchConnect(&B, dst,
				  bp.getNumerator(),
				  bp.getDenominator());
				probList.push_back(bConn);
			}
		errs() << "##########################################33\n";

		}
	}
	errs() << "\n~~Raw edge results: \n";
	for (BranchConnect *c : rawProbList) {
//		BranchConnect *c  = *it;
		BasicBlock *sb = c->getSourceBlock();
		BasicBlock *db = c->getDestinationBlock();
		errs() << (long)sb << " --> " << (long)db << " : " << c->getProbability() << "\n";
	}
	errs() << "\n~~Summed results: \n";
	for (BranchConnect *c : probList) {
		BasicBlock *sb = c->getSourceBlock();
		BasicBlock *db = c->getDestinationBlock();
		errs() << (long)sb << " --> " << (long)db << " : " << c->getProbability() << "\n";
	}
	return false;
}

char PathProbabilities::ID = 0;
static RegisterPass<PathProbabilities> X("pprobs", "");
