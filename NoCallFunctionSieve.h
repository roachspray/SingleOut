#ifndef _NOCALLFUNCTIONSIEVEPASS_H
#define	_NOCALLFUNCTIONSIEVEPASS_H

struct NoCallFunctionSievePass : ModulePass {
	static char ID;
	NoCallFunctionSievePass() : ModulePass(ID) {}
	virtual bool runOnModule(Module &);
};
#endif
