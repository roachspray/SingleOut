#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/ADT/SmallVector.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <map>
#include <set>
#include <vector>

using namespace llvm;

#include "FindInstructionsByLocation.h"

cl::opt<std::string> FileTargets("location-file", cl::desc("file and lines"),
  cl::init(""));

bool
FindInstructionsByLocation::runOnModule(Module &M)
{
	Function *keep = NULL;
	bool modified = false;

	std::vector<std::map<std::string, std::string>> vRet;
	unsigned scRet = 0;

	if (FileTargets == "") {
		errs() << "No file target listing\n";
		return false;
	}
	std::ifstream fH(FileTargets);

	std::vector<std::pair<std::string, int>> targets;
	std::string targetFileLine;
	while (1) {
		std::getline(fH, targetFileLine);
		if (targetFileLine == "") {
			break;
		}
		std::string targetFile;
		std::string targetLine;

		std::stringstream origConv(targetFileLine);
		origConv >> targetFile >> targetLine;
		auto p = std::make_pair(targetFile, std::stoi(targetLine, nullptr, 10));
		targets.push_back(p);
	}
	// I'm guessing there is a better way to do this, but that is not my current concern
	for (Function &F :  M) {
		for (BasicBlock &B : F) {
			for (Instruction &I: B) {
				if (I.hasMetadata()) {
					SmallVector< std::pair< unsigned, MDNode *>, 16> MDs;
					I.getAllMetadata(MDs);
					for (auto ii = MDs.begin(); ii != MDs.end(); ++ii) {
						std::pair<unsigned, MDNode *> m = *ii;
						MDNode *md = std::get<1>(m);
						if (isa<DILocation>(md)) {
							DILocation *d = cast<DILocation>(md);
							if (auto *SP = dyn_cast<DISubprogram>(d->getRawScope())) {
								auto df = SP->getFile();
								std::string thisFile = df->getFilename().str();
								unsigned thisLine = d->getLine();	
								bool bothare = false;
								for (auto ek = targets.begin() ; ek != targets.end() ; ++ek) {
									if (ek->first == thisFile && ek->second == thisLine) {
										bothare = true;
									}
								}
								if (bothare == false) {
									continue;
								}
								LLVMContext &C = I.getContext();
								MDNode *N = MDNode::get(C, MDString::get(C, "TARGET"));
								I.setMetadata("SLICE", N);
							}
							
						}
					}
					
				}
			}
		}
	}
/*
	DebugInfoFinder Finder;
	Finder.processModule(M);
	for (DebugInfoFinder::iterator i = Finder.subprogram_begin(),
	  e = Finder.subprogram_end(); i != e; ++i) {
		DISubprogram S(*i);

		if (S.getFunction() == F) {
			errs() << S.getLineNumber(); << "\n";
		}
	}

	if (FileName == "" || FileLine == -1) {
		errs() << "Specify file and line number\n";
		return false;
	}
 */

	return true;
}

char FindInstructionsByLocation::ID = 0;
static RegisterPass<FindInstructionsByLocation> X("instbyloc", "InstructionsByLocation");
