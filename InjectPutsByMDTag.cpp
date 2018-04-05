#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
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

#include "InjectPutsByMDTag.h"

bool
InjectPutsByMDTag::runOnModule(Module &M)
{
	LLVMContext &C = M.getContext();
	Constant *cpf = M.getOrInsertFunction("puts", IntegerType::get(C, 32), Type::getInt8PtrTy(C, 0), NULL);
	Function *pf = cast<Function>(cpf);
	pf->setCallingConv(CallingConv::C);

	Constant *outStr = ConstantDataArray::getString(C, "__HI__", true);
	GlobalVariable *gvOutStr = new GlobalVariable(M,
	  outStr->getType(), true,
	  GlobalValue::PrivateLinkage, outStr);
	std::string naming = "__INJECT_STR";
	gvOutStr->setName(naming);
	gvOutStr->setAlignment(1);

	// I'm guessing there is a better way to do this, but that is not my current concern
	for (Function &F :  M) {
		for (BasicBlock &B : F) {
			for (Instruction &I: B) {
				if (I.hasMetadata() && I.getMetadata("SLICE") != NULL) {
	ArrayType *at = ArrayType::get(Type::getInt8Ty(C), naming.size()+1);
	std::vector<Value *> idxs;
	idxs.push_back(
	  Constant::getIntegerValue(Type::getInt32Ty(C),
	  APInt(32, 0, false))
	);
   	idxs.push_back(
   	  Constant::getIntegerValue(Type::getInt32Ty(C),
	  APInt(32, 0, false))
	);
	Constant *useGv = ConstantExpr::getInBoundsGetElementPtr(at,
	  gvOutStr, idxs);
	CallInst::Create(pf,
   		{ useGv },
	       Twine::createNull(),
   		&I);
					
				}
			}
		}
	}
	return true;
}

char InjectPutsByMDTag::ID = 0;
static RegisterPass<InjectPutsByMDTag> X("puts-mdtag", "Puts by MD tag");
