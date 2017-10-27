/*
 * not even prototype front end to finding functions that some static
 * analysis thinks should have some assume/assert compile time DbC 
 * checks.
 *
 * more lame arr code
 */
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

#include <set>

using namespace llvm;

#include "PossiblyCheck.h"

bool
PossiblyCheck::scCheckAll(Function *F)
{
	return true;
}

bool
PossiblyCheck::scCheckNone(Function *F)
{
	return false;
}


bool
PossiblyCheck::runOnModule(Module &M)
{
	
	for (Function &F : M) {
		if (F.hasName() == false) {
			continue;
		}
		std::string Fname = F.getName().str();
		errs() << Fname << ":\n";
		for (auto ci = shouldCheckMap.begin(); ci != shouldCheckMap.end();
		  ++ci) {
			std::string scName = ci->first;
			errs() << "   Running check test:  " << scName  << "  ";
			shouldCheck scFunc = ci->second;			
			if (scFunc(&F) == true) {
				errs() << "(SHOULD CHECK)\n";
			} else {
				errs() << "(NO COMMENT)\n";
			} 
		}
	}	
	return false;
}

char PossiblyCheck::ID = 0;
static RegisterPass<PossiblyCheck> X("possibly-check", "Possibly add DbC checks");
