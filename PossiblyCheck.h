#ifndef _POSSIBLYCHECK_H
#define	_POSSIBLYCHECK_H

#include <map>

typedef bool (*shouldCheck)(llvm::Function *);

struct PossiblyCheck : ModulePass {
	static char ID;
	PossiblyCheck() : ModulePass(ID) {}
	virtual bool runOnModule(Module &);

private:
	static bool scCheckAll(Function *);
	static bool scCheckNone(Function *);
	std::map<std::string, shouldCheck> shouldCheckMap = {
		std::make_pair("check all", scCheckAll),
		std::make_pair("check none", scCheckNone)
	};
};
#endif
