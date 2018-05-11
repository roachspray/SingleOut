#ifndef _PATHPROBS_H
#define	_PATHPROBS_H

struct PathProbabilities : ModulePass {
	static char ID;
	PathProbabilities() : ModulePass(ID) {}
	virtual bool runOnModule(Module &);
	void getAnalysisUsage(AnalysisUsage &AU) const;
};
#endif
