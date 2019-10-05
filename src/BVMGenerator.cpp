#include <ShaderDebugger/BVMGenerator.h>

// Glslang includes
#include <glslang/MachineIndependent/SymbolTable.h>
#include <glslang/Include/Common.h>
#include <glslang/Include/revision.h>

#include <fstream>
#include <iomanip>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <vector>

namespace sd {

	class TGlslangToBVMTraverser : public glslang::TIntermTraverser {
	public:
		TGlslangToBVMTraverser(const glslang::TIntermediate*);
		virtual ~TGlslangToBVMTraverser() { }

		bool visitAggregate(glslang::TVisit, glslang::TIntermAggregate*);
		bool visitBinary(glslang::TVisit, glslang::TIntermBinary*);
		void visitConstantUnion(glslang::TIntermConstantUnion*);
		bool visitSelection(glslang::TVisit, glslang::TIntermSelection*);
		bool visitSwitch(glslang::TVisit, glslang::TIntermSwitch*);
		void visitSymbol(glslang::TIntermSymbol* symbol);
		bool visitUnary(glslang::TVisit, glslang::TIntermUnary*);
		bool visitLoop(glslang::TVisit, glslang::TIntermLoop*);
		bool visitBranch(glslang::TVisit visit, glslang::TIntermBranch*);

		void dump(std::vector<uint8_t>& out);

	protected:
		TGlslangToBVMTraverser(TGlslangToBVMTraverser&);
		TGlslangToBVMTraverser& operator=(TGlslangToBVMTraverser&);
	};

	void BVMGenerator::Translate(const glslang::TIntermediate& intermediate, std::vector<uint8_t>& out)
	{
		TIntermNode* root = intermediate.getTreeRoot();

		if (root == 0)
			return;

		glslang::GetThreadPoolAllocator().push();

		TGlslangToBVMTraverser it(&intermediate);
		root->traverse(&it);
		it.dump(out);

		glslang::GetThreadPoolAllocator().pop();
	}
}