#pragma once

#include <glsl-parser/parser.h>
#include <aGen/aGen.hpp>

#include <unordered_map>
#include <string>
#include <vector>
#include <stack>

#include "Translator.h"

namespace sd
{
	class GLSLTranslator : public Translator
	{
	public:
		virtual bool Parse(ShaderType type, const std::string& source, std::string = "main");


	private:
		void translateOperator(int op);
		void translateIntConstant(glsl::astIntConstant *expression);
		void translateUIntConstant(glsl::astUIntConstant *expression);
		void translateFloatConstant(glsl::astFloatConstant *expression);
		void translateDoubleConstant(glsl::astDoubleConstant *expression);
		void translateBoolConstant(glsl::astBoolConstant *expression);
		void translateArraySize(const glsl::vector<glsl::astConstantExpression*> &arraySizes);
		void translateStorage(int storage);
		void translateAuxiliary(int auxiliary);
		void translateMemory(int memory);
		void translatePrecision(int precision);
		void translateGlobalVariable(glsl::astGlobalVariable *variable);
		void translateVariableIdentifier(glsl::astVariableIdentifier *expression);
		void translateFieldOrSwizzle(glsl::astFieldOrSwizzle *expression);
		void translateArraySubscript(glsl::astArraySubscript *expression);
		void translateFunctionCall(glsl::astFunctionCall *expression);
		void translateConstructorCall(glsl::astConstructorCall *expression);
		void translateDeclarationVariable(glsl::astFunctionVariable *variable);
		void translatePostIncrement(glsl::astPostIncrementExpression *expression);
		void translatePostDecrement(glsl::astPostDecrementExpression *expression);
		void translateUnaryMinus(glsl::astUnaryMinusExpression *expression);
		void translateUnaryPlus(glsl::astUnaryPlusExpression *expression);
		void translateUnaryBitNot(glsl::astUnaryBitNotExpression *expression);
		void translateUnaryLogicalNot(glsl::astUnaryLogicalNotExpression *expression);
		void translatePrefixIncrement(glsl::astPrefixIncrementExpression *expression);
		void translatePrefixDecrement(glsl::astPrefixDecrementExpression *expression);
		void translateAssign(glsl::astAssignmentExpression *expression);
		void translateSequence(glsl::astSequenceExpression *expression);
		void translateOperation(glsl::astOperationExpression *expression);
		void translateTernary(glsl::astTernaryExpression *expression);
		void translateExpression(glsl::astExpression *expression);
		void translateCompoundStatement(glsl::astCompoundStatement *statement);
		void translateDeclarationStatement(glsl::astDeclarationStatement *statement);
		void translateExpressionStatement(glsl::astExpressionStatement *statement);
		void translateIfStetement(glsl::astIfStatement *statement);
		void translateSwitchStatement(glsl::astSwitchStatement *statement);
		void translateCaseLabelStatement(glsl::astCaseLabelStatement *statement);
		void translateWhileStatement(glsl::astWhileStatement *statement);
		void translateDoStatement(glsl::astDoStatement *statement);
		void translateForStatement(glsl::astForStatement *statement);
		void translateContinueStatement();
		void translateBreakStatement();
		void translateReturnStatement(glsl::astReturnStatement *statement);
		void translateDiscardStatement();
		void translateStatement(glsl::astStatement *statement);
		void translateFunction(glsl::astFunction *function);
		void translateStructure(glsl::astStruct *structure);
		void translate(glsl::astTU *tu);

	private:
		std::unordered_map<std::string, glsl::astConstantExpression*> m_initInMain;
		std::unordered_map<std::string, glsl::vector<glsl::astConstantExpression*>> m_initArraysInMain; // global arrays
		std::string m_currentFunction, m_entryFunction;

		std::stack<std::vector<size_t>> m_breaks;	// contains all break; IDs gathered from m_gen.Function.Goto()
		std::stack<int> m_continueAddr;				// m_continueAddr.size() != 0 when 'continue' can be called, points to address to go to when continue is called
		bool m_isSet, m_usePointer;
		bool m_caseIfDefault;
		size_t m_caseIfAddr;
	};
}