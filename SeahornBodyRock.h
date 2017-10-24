#ifndef _SEAHORNBODYROCK_H
#define	_SEAHORNBODYROCK_H

struct SeahornBodyRock : ModulePass {
	static char ID;
	SeahornBodyRock() : ModulePass(ID) {}
	virtual bool runOnModule(Module &);
};
#endif
