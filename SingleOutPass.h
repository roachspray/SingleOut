#ifndef _SINGLEOUTPASS_H
#define	_SINGLEOUTPASS_H

struct SingleOutPass : ModulePass {
	static char ID;
	SingleOutPass() : ModulePass(ID) {}
	virtual bool runOnModule(Module &);
};
#endif
