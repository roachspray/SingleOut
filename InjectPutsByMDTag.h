#ifndef _INJECTPUTSBYMDTAG_H
#define	_INJECTPUTSBYMDTAG_H

struct InjectPutsByMDTag : ModulePass {
	static char ID;
	InjectPutsByMDTag() : ModulePass(ID) {}
	virtual bool runOnModule(Module &);
};
#endif
