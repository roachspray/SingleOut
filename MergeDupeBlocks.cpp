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

#include "MergeDupeBlocks.h"

void
MergeDupeBlocks::getAnalysisUsage(AnalysisUsage &AU) const
{
	AU.setPreservesCFG();
	AU.addRequired<BranchProbabilityInfoWrapperPass>();
}

static BasicBlock *
isSimpleBlock(BasicBlock *b)
{
	if (std::distance(b->begin(), b->end()) != 1) {
		return NULL;
	}
	TerminatorInst *ti = b->getTerminator();
	if (!isa<BranchInst>(ti)) {
		return NULL;
	}
	BranchInst *bi = cast<BranchInst>(ti);
	if (bi->isConditional()) {
		return NULL;
	}
	BasicBlock *v = bi->getSuccessor(0);
	auto it = v->begin();
	Instruction &I = *it;
	if (isa<PHINode>(&I)) {
		return NULL;
	}
	return v;
}

bool
MergeDupeBlocks::runOnModule(Module &M)
{

	for (Function &F :  M) {
		if (F.isDeclaration()) {
			continue;
		}

		BranchProbabilityInfo &bpi =   \
		  getAnalysis<BranchProbabilityInfoWrapperPass>(F).getBPI();

		for (BasicBlock &B : F) {
			for (Instruction &I : B) {
				if (!isa<SwitchInst>(I)) {
					continue;
				}
				
				SwitchInst *si = cast<SwitchInst>(&I);
				for (auto it = si->case_begin(); 
				  it != si->case_end(); ++it) {
					SwitchInst::CaseIt ch = *it;
					BasicBlock *suc = ch.getCaseSuccessor();
					BasicBlock *nextStep = 	isSimpleBlock(suc);
					if (nextStep == NULL) {
						continue;
					}
					ch.setSuccessor(nextStep);
					/*
					 * We have a block that is an unconditional
					 * branch only.. so we return the destination
					 * of that branch.
					 * With that we check for other simple cases..
					 */
					for (auto it2 = si->case_begin(); 
					  it2 != si->case_end(); ++it2) {
						SwitchInst::CaseIt ch1 = *it2;
						BasicBlock *suc1 = ch1.getCaseSuccessor();
						if (suc1 == suc) {
							continue;
						}
						BasicBlock *nextStep2 = isSimpleBlock(suc1);
						if (nextStep2 == NULL) {
							if (nextStep == nextStep2) {
								ch1.setSuccessor(nextStep);
							}
						}
					}
				}

			}			
		}
	}
	return false;
}

char MergeDupeBlocks::ID = 0;
static RegisterPass<MergeDupeBlocks> X("mergeblocks", "");
