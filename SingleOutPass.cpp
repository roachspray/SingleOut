/*
 * Keeps the definition of the specified function and
 * declarations of any functions it may call. Erases
 * all other functions.
 *
 * more lame arr code
 */
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

#include <set>

using namespace llvm;

#include "SingleOutPass.h"

cl::opt<std::string> FunctionToKeep("single-out-function", cl::desc("<function>"),
  cl::init(""));
cl::opt<bool> SOVerbose("single-out-verbose", cl::desc("verbose yo"),
  cl::init(false));

bool
SingleOutPass::runOnModule(Module &M)
{
	Function *keep = NULL;
	bool modified = false;

	if (FunctionToKeep == "") {
		errs() << "SingleOutPass: specify a function to keep\n";
		return false;
	}

	keep = M.getFunction(FunctionToKeep);
	if (keep == NULL) {
		errs() << "SingleOutPass: unable to find function: '" << \
		  FunctionToKeep << "'\n";
		return false;
	}

	std::set<Function *> keepDecl;
	for (BasicBlock &B : *keep) {
		for (Instruction &I : B) {
			if (isa<CallInst>(I) || isa<InvokeInst>(I)) {
				CallSite cs(&I);
				keepDecl.insert(cs.getCalledFunction());
			}
		}
	}

	std::vector<Function *> removeThese;
	for (Function &F : M) {
		Function *pF = &F;
		if (pF == keep) {
			continue;
		}
		bool kd = false;
		for (auto kdi = keepDecl.begin(); kdi != keepDecl.end(); ++kdi) {
			Function *called = *kdi;
			if (called == pF) {
				kd = true;
				break;
			}
		}
		pF->deleteBody();
		modified = true;	
		if (!kd) {
			removeThese.push_back(pF);
		}
			
	}
	for (auto rm = removeThese.begin(); rm != removeThese.end(); ++rm) {
		Function *rmF = *rm;
		rmF->eraseFromParent();
	}
	return modified;
}

char SingleOutPass::ID = 0;
static RegisterPass<SingleOutPass> X("single-out", "SingleOut");
