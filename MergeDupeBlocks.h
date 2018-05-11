#ifndef _MERGEDUPEBLOCKS_H
#define	_MERGEDUPEBLOCKS_H

struct MergeDupeBlocks : ModulePass {
	static char ID;
	MergeDupeBlocks() : ModulePass(ID) {}
	virtual bool runOnModule(Module &);
	void getAnalysisUsage(AnalysisUsage &AU) const;
};
#endif
