/*
 * Removes all functions in module with no CallInst or 
 * InvokeInst instruction.
 *
 * more lame arr code
 */
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

#include <set>
#include <algorithm>

using namespace llvm;

#include "NoCallFunctionSieve.h"

cl::opt<bool> NCFVerbose("no-call-sieve-verbose", cl::desc("verbose yo"),
  cl::init(false));

cl::opt<unsigned> NCCallLevel("no-call-sieve-level", cl::desc("level"),
  cl::init(0));

cl::opt<bool> NCGreaterThan("no-call-greater", cl::desc("fns with counts > n"),
  cl::init(false));

static inline std::vector<std::string>
do_union(std::vector<std::string> keepDecl, std::vector<std::string> calledSet)
{
	std::vector<std::string> d;
	std::set_union(keepDecl.begin(), keepDecl.end(), calledSet.begin(),
	  calledSet.end(), std::back_inserter(d));
	return d;
}

bool
NoCallFunctionSievePass::runOnModule(Module &M)
{
	std::vector<Function *> removeThese;
	bool modified = false;

	std::vector<std::string> keepDecl;


	for (Function &F : M) {
		Function *pF = &F;
		std::set<std::string> calledSet;
		bool keep = true;

		for (BasicBlock &B : F) {
			for (Instruction &I : B) {
				if (isa<CallInst>(I) || isa<InvokeInst>(I)) {
					CallSite cs(&I);
					calledSet.insert(cs.getCalledFunction()->getName().str());
				}
			}
		}
		if (!((NCGreaterThan == false && calledSet.size() == NCCallLevel) || \
		  (NCGreaterThan == true && calledSet.size() > NCCallLevel))) {	
			removeThese.push_back(pF);
			continue;
		}
		std::vector<std::string> calledV;
		std::copy(calledSet.begin(), calledSet.end(), std::back_inserter(calledV));
		keepDecl = do_union(keepDecl, calledV);
	}
	for (auto fi = removeThese.begin(); fi != removeThese.end(); ++fi) {
		Function *pF = *fi;
		std::string remName = pF->getName().str();
		bool keepIt = false;
		for (auto k = keepDecl.begin(); k != keepDecl.end(); ++k) {
			std::string s = *k;
			if (remName == s) {
				keepIt = true;
				break;
			}
		}	
		if (keepIt) {
			pF->deleteBody();
		} else {
			pF->eraseFromParent();
		}	
	}
	return modified;
}

char NoCallFunctionSievePass::ID = 0;
static RegisterPass<NoCallFunctionSievePass> X("no-call-sieve", "no call sieve");
