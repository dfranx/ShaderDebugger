#pragma once
#include <hlslparser/HLSLTree.h>
#include <ShaderDebugger/Compiler.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <stack>

namespace M4 { class Allocator; class Logger; class HLSLNode; }
namespace sd
{
	class HLSLCompiler : public Compiler
	{
	public:
		HLSLCompiler();
		~HLSLCompiler();

		virtual bool Parse(ShaderType type, const std::string& source, std::string entry = "main");

		virtual void ClearImmediate();
		virtual void AddImmediateGlobalDefinition(Variable var);

	private:
		bool translate(M4::HLSLTree* tree);
		void translateExpressionList(M4::HLSLExpression* expression);
		void translateExpression(M4::HLSLExpression* expression);
		void translateIdentifierExpression(M4::HLSLIdentifierExpression* expression);
		void translateCastingExpression(M4::HLSLCastingExpression* expression);
		void translateLiteralExpression(M4::HLSLLiteralExpression* expression);
		void translateUnaryExpression(M4::HLSLUnaryExpression* expression);
		void translateBinaryExpression(M4::HLSLBinaryExpression* expression);
		void translateAssignExpression(M4::HLSLBinaryExpression* expression);
		void translateOperator(M4::HLSLBinaryOp op);
		void translateTernary(M4::HLSLConditionalExpression* expression);
		void translateMemberAccess(M4::HLSLMemberAccess* expr);
		void translateMethodCall(M4::HLSLMethodCall* expr);
		void translateAttributes(M4::HLSLAttribute* expr);
		void translateArrayAccess(M4::HLSLArrayAccess* expr);
		void translateFunctionCall(M4::HLSLFunctionCall* expr);

		void translateStatements(M4::HLSLStatement* statement);
		void translateStructure(M4::HLSLStruct* str);
		void translateArguments(M4::HLSLArgument* argument);
		void translateFunction(M4::HLSLFunction* func);
		void translateReturnStatement(M4::HLSLReturnStatement* ret);
		void translateDeclaration(M4::HLSLDeclaration* declaration);
		void translateBreakStatement(M4::HLSLBreakStatement* brk);
		void translateContinueStatement(M4::HLSLContinueStatement* brk);
		void translateIfStatement(M4::HLSLIfStatement* ifStmt);
		void translateForStatement(M4::HLSLForStatement* forStmt);

	private:
		class ExpressionType
		{
		public:
			ExpressionType() { Name = "float"; Type = ag::Type::Float; Columns = Rows = 1; }
			ExpressionType(const std::string& name, ag::Type type, int cols = 1, int rows = 1)
			{
				Name = name; Type = type; Columns = cols; Rows = rows;
			}

			bool operator==(const ExpressionType& obj) {
				return Name == obj.Name && Type == obj.Type && Columns == obj.Columns && Rows == obj.Rows;
			}
			bool operator!=(const ExpressionType& obj) {
				return !(*this == obj);
			}

			std::string Name;
			ag::Type Type;
			int Columns, Rows;
		};

		ExpressionType m_convertType(const M4::HLSLBaseType& type, const char* userDefined = NULL);
		ExpressionType m_convertType(const M4::HLSLType& type);
		ExpressionType m_convertType(const std::string& type);
		void m_generateConvert(ExpressionType etype, int args = 1);
		bool m_isTypeActuallyStruct(ExpressionType type);

		Function m_matchFunction(const char* name, int argCount, M4::HLSLExpression* args);

		int m_lastLineSaved; // dont add OpCode::DebugLine for every expression, but rather only when astNode->line != m_lastLineSaved
		
		inline void m_exportLine(const M4::HLSLNode* node, bool force = false)
		{
			if (!node)
				return;

			if ((m_lastLineSaved != node->line || force) && m_curFuncData != nullptr) {
				m_gen.Function.DebugLineNumber(node->line);
				m_lastLineSaved = node->line;
			}
		}

		bool m_isInsideBuffer;

		std::vector<std::pair<std::string, std::pair<M4::HLSLType, std::string>>> m_immGlobals;

		std::unordered_map<std::string, std::string> m_initObjsInMain;
		std::unordered_map<std::string, M4::HLSLExpression*> m_initInMain;
		std::unordered_map<std::string, M4::HLSLExpression*> m_initArraysInMain; // global arrays
		std::string m_currentFunction, m_entryFunction;

		Function* m_curFuncData;

		std::stack<std::vector<size_t>> m_breaks;
		std::stack<int> m_continueAddr;				// m_continueAddr.size() != 0 when 'continue' can be called, points to address to go to when continue is called
		bool m_isSet, m_usePointer;
		bool m_caseIfDefault;
		size_t m_caseIfAddr;

		std::unordered_map<std::string, std::vector<int>> m_builtInFuncsPtrs;
		void m_buildFuncArgPtrs();

		// immediate mode function definitions
		std::vector<M4::HLSLFunction*> m_immFuncs;
		std::vector<M4::HLSLStruct*> m_immStructs;
		void m_freeImmediate();
	};

}