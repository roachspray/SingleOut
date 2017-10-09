/*
 * Removes all functions in module with no CallInst or 
 * InvokeInst instruction.
 *
 * more lame arr code
 */
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

#include "NoCallFunctionSieve.h"

cl::opt<bool> NCFVerbose("no-call-sieve-verbose", cl::desc("verbose yo"),
  cl::init(false));

bool
NoCallFunctionSievePass::runOnModule(Module &M)
{
	std::vector<Function *> removeThese;
	bool modified = false;

	for (Function &F : M) {
		Function *pF = &F;
		bool keep = true;
		for (BasicBlock &B : F) {
			for (Instruction &I : B) {
				if (isa<CallInst>(I) || isa<InvokeInst>(I)) {
					keep = false;
					goto you_know_you_love_gotos;
				}
			}
		}
you_know_you_love_gotos:
		if (keep == false) {
			removeThese.push_back(pF);
		}
	}
	for (auto fi = removeThese.begin(); fi != removeThese.end(); ++fi) {
		Function *pF = *fi;
		pF->eraseFromParent();
	}
	return modified;
}

char NoCallFunctionSievePass::ID = 0;
static RegisterPass<NoCallFunctionSievePass> X("no-call-sieve", "no call sieve");
