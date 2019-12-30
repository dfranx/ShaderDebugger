#pragma once

#include <glsl-parser/parser.h>
#include <aGen/aGen.hpp>

#include <unordered_map>
#include <string>
#include <vector>
#include <stack>

#include <ShaderDebugger/Translator.h>

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

		inline bool isTypeActuallyStruct(const std::string& t) {
			return !(t == "void" || t == "bool" || t == "int" || t == "uint" || t == "float" || t == "double");
		}


		class ExpressionType
		{
		public:
			ExpressionType() { Name = "float"; Type = ag::Type::Float; Columns = Rows = 1; }
			ExpressionType(const std::string& name, ag::Type type, int cols, int rows)
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
		}; // example: "vec4", ag::Type::Float, 4, 1

		ExpressionType evaluateExpressionType(glsl::astExpression* expr);

		Function matchFunction(const char* name, const std::vector<glsl::astConstantExpression*>& params);
		void generateConvert(ExpressionType etype);

	private:
		int m_lastLineSaved; // dont add OpCode::DebugLine for every expression, but rather only when astNode->line != m_lastLineSaved
		template<typename T>
		inline void m_exportLine(glsl::astNode<T>* node, bool force = false)
		{
			if (m_lastLineSaved != node->line || force) {
				m_gen.Function.DebugLineNumber(node->line);
				m_lastLineSaved = node->line;
			}
		}

		ExpressionType m_convertExprType(const std::string& str);
		ExpressionType m_mergeExprType(int op, const ExpressionType& left, const ExpressionType& right);
		ag::Type m_mergeBaseType(ag::Type left, ag::Type right);

		ag::Type m_convertBaseType(const std::string& str);

		std::unordered_map<std::string, std::string> m_initObjsInMain;
		std::unordered_map<std::string, glsl::astConstantExpression*> m_initInMain;
		std::unordered_map<std::string, glsl::vector<glsl::astConstantExpression*>> m_initArraysInMain; // global arrays
		std::string m_currentFunction, m_entryFunction;

		Function* m_curFuncData;

		std::stack<std::vector<size_t>> m_breaks;	// contains all break; IDs gathered from m_gen.Function.Goto()
		std::stack<int> m_continueAddr;				// m_continueAddr.size() != 0 when 'continue' can be called, points to address to go to when continue is called
		bool m_isSet, m_usePointer;
		bool m_caseIfDefault;
		size_t m_caseIfAddr;

		std::unordered_map<std::string, std::vector<int>> m_builtInFuncsPtrs;
		void m_buildFuncArgPtrs();

		bool m_writeIndexDepth;
		int m_indexDepth;
	};
}