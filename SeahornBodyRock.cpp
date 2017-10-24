/*
 * only purpose is to keep orig.main and main and deleteBody all others
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

#include "SeahornBodyRock.h"

bool
SeahornBodyRock::runOnModule(Module &M)
{
	for (Function &F : M) {
		if (F.hasName()) {
			std::string s = F.getName().str();
			if (s == "main" || s == "orig.main") {
				continue;
			}
		}
		F.deleteBody();
	}	
	return true;
}

char SeahornBodyRock::ID = 0;
static RegisterPass<SeahornBodyRock> X("seahorn-body-rock", "Body rockin' yauh");
