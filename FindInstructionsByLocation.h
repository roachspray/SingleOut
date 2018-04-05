#ifndef _FINDINSTRUCTIONSFROMLOCATION_H
#define	_FINDINSTRUCTIONSFROMLOCATION_H

struct FindInstructionsByLocation : ModulePass {
	static char ID;
	FindInstructionsByLocation() : ModulePass(ID) {}
	virtual bool runOnModule(Module &);
};
#endif
