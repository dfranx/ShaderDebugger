#include <ShaderDebugger/GLSLTranslator.h>
#include <ShaderDebugger/GLSLLibrary.h>
#include <string.h>

#undef KEYWORD
#define KEYWORD(X) #X,
static const char *kTypes[] = {
	#include <glsl-parser/lexemes.h>
};

namespace sd
{
	bool GLSLTranslator::Parse(ShaderType type, const std::string& source, std::string entry)
	{
		PropertyGetter = Library::GLSLswizzle;
		m_lastLineSaved = -1;
		m_isSet = false;
		m_usePointer = false;
		m_caseIfDefault = false;
		m_caseIfAddr = 0;
		m_entryFunction = entry;
		m_gen.SetHeader(0, 2);

		int shaderType = glsl::astTU::kVertex;
		if (type == ShaderType::Pixel)
			shaderType = glsl::astTU::kFragment;

        glsl::parser p(source.c_str(), "memory");
        glsl::astTU *tu = p.parse(shaderType);
        if (tu) translate(tu);
        else return false;

		return true;
	}
	
	void GLSLTranslator::translateOperator(int op) {
		switch (op)
		{
		case glsl::kOperator_increment: m_gen.Function.Increment(); break;
		case glsl::kOperator_decrement: m_gen.Function.Decrement(); break;
		case glsl::kOperator_bit_not: m_gen.Function.BitNot(); break;
		case glsl::kOperator_logical_not: m_gen.Function.Not(); break;
		case glsl::kOperator_multiply: m_gen.Function.Multiply(); break;
		case glsl::kOperator_divide: m_gen.Function.Divide(); break;
		case glsl::kOperator_modulus: m_gen.Function.Modulo(); break;
		case glsl::kOperator_plus: m_gen.Function.Add(); break;
		case glsl::kOperator_minus: m_gen.Function.Subtract(); break;
		case glsl::kOperator_shift_left: m_gen.Function.BitLeftShift(); break;
		case glsl::kOperator_shift_right: m_gen.Function.BitRightShift(); break;
		case glsl::kOperator_less: m_gen.Function.Less(); break;
		case glsl::kOperator_greater: m_gen.Function.Greater(); break;
		case glsl::kOperator_less_equal: m_gen.Function.LessEqual(); break;
		case glsl::kOperator_greater_equal: m_gen.Function.GreaterEqual(); break;
		case glsl::kOperator_equal: m_gen.Function.Equal(); break;
		case glsl::kOperator_not_equal: m_gen.Function.NotEqual(); break;
		case glsl::kOperator_bit_and: m_gen.Function.BitAnd(); break;
		case glsl::kOperator_bit_xor: m_gen.Function.BitXor(); break;
		case glsl::kOperator_bit_or: m_gen.Function.BitOr(); break;
		case glsl::kOperator_logical_and: m_gen.Function.BoolAnd(); break;
		case glsl::kOperator_logical_or: m_gen.Function.BoolOr(); break;
		case glsl::kOperator_assign: m_gen.Function.Assign(); break;
		
		default:
			break;
		}
	}

	void GLSLTranslator::translateIntConstant(glsl::astIntConstant *expression) {
		m_gen.Function.PushStack((int)expression->value);
	}

	void GLSLTranslator::translateUIntConstant(glsl::astUIntConstant *expression) {
		m_gen.Function.PushStack((int)expression->value);
	}

	void GLSLTranslator::translateFloatConstant(glsl::astFloatConstant *expression) {
		m_gen.Function.PushStack((float)expression->value);
	}

	void GLSLTranslator::translateDoubleConstant(glsl::astDoubleConstant *expression) {
		m_gen.Function.PushStack((float)expression->value); // TODO: double?
	}

	void GLSLTranslator::translateBoolConstant(glsl::astBoolConstant *expression) {
		m_gen.Function.PushStack((char)expression->value);
	}

	void GLSLTranslator::translateArraySize(const glsl::vector<glsl::astConstantExpression*> &arraySizes) {
		for (size_t i = 0; i < arraySizes.size(); i++)
			translateExpression(arraySizes[i]);
	}

	void GLSLTranslator::translateStorage(int storage) {
		switch (storage) {
		case glsl::kConst: break;
		case glsl::kIn: break;
		case glsl::kOut: break;
		case glsl::kAttribute: break;
		case glsl::kUniform: break;
		case glsl::kVarying: break;
		case glsl::kBuffer: break;
		case glsl::kShared: break;
		}
	}

	void GLSLTranslator::translateAuxiliary(int auxiliary) {
		// TODO
		switch (auxiliary) {
		case glsl::kCentroid: break;
		case glsl::kSample: break;
		case glsl::kPatch: break;
		}
	}

	void GLSLTranslator::translateMemory(int memory) {
		// TODO
		if (memory & glsl::kCoherent) {}
		if (memory & glsl::kVolatile) {}
		if (memory & glsl::kRestrict) {}
		if (memory & glsl::kReadOnly) {}
		if (memory & glsl::kWriteOnly) {}
	}

	void GLSLTranslator::translatePrecision(int precision) {
		// TODO
		switch (precision) {
			case glsl::kLowp: break;
			case glsl::kMediump: break;
			case glsl::kHighp: break;
		}
	}

	void GLSLTranslator::translateGlobalVariable(glsl::astGlobalVariable *variable) {
		m_globals.push_back(Variable());

		Variable& var = m_globals[m_globals.size()-1];

		// layout qualifiers
		glsl::vector<glsl::astLayoutQualifier*> &qualifiers = variable->layoutQualifiers;
		if (variable->layoutQualifiers.size()) {
			for (size_t i = 0; i < qualifiers.size(); i++) {
				glsl::astLayoutQualifier *qualifier = qualifiers[i];
				if (qualifier->initialValue->type == glsl::astExpression::kIntConstant) {

					// TODO: save other layout qualifiers too
					if (strcmp(qualifier->name, "location") == 0)
						var.InputSlot = ((glsl::astIntConstant*)qualifier->initialValue)->value;
					printf("[DEBUG] Setting layout qualifier...\n");
				}
			}
		}

		var.Smooth = false;
		var.Flat = false;
		var.NoPerspective = false;
		switch (variable->interpolation) {
		case glsl::kSmooth:
			var.Smooth = true;
			break;
		case glsl::kFlat:
			var.Flat = true;
			break;
		case glsl::kNoPerspective:
			var.NoPerspective = true;
			break;
		}

		switch (variable->storage) {
		case glsl::kConst: var.Storage = Variable::StorageType::Constant; break;
		case glsl::kIn: var.Storage = Variable::StorageType::In; break;
		case glsl::kOut: var.Storage = Variable::StorageType::Out; break;
		case glsl::kAttribute: var.Storage = Variable::StorageType::Attribute; break;
		case glsl::kUniform: var.Storage = Variable::StorageType::Uniform; break;
		case glsl::kVarying: var.Storage = Variable::StorageType::Varying; break;
		case glsl::kBuffer: var.Storage = Variable::StorageType::Buffer; break;
		case glsl::kShared: var.Storage = Variable::StorageType::Shared; break;
		}
		

		glsl::astVariable* varData = (glsl::astVariable*)variable;
		var.Name = varData->name;
		if (variable->baseType->builtin)
			var.Type = kTypes[((glsl::astBuiltin*)variable->baseType)->type];
		else
			var.Type = ((glsl::astStruct*)variable->baseType)->name;

		if (isTypeActuallyStruct(var.Type))
			m_initObjsInMain[var.Name] = var.Type;

		// TODO: global arrays
		if (variable->isArray)
			m_initArraysInMain[var.Name] = variable->arraySizes;

		// generate bytecode
		var.ID = m_gen.AddGlobal(varData->name);
		
		if (variable->initialValue)
			m_initInMain[varData->name] = variable->initialValue;
	}

	void GLSLTranslator::translateVariableIdentifier(glsl::astVariableIdentifier *expression) {
		glsl::astVariable* vdata = (glsl::astVariable*)expression->variable;
		
		bool found = false;

		// globals
		for (int i = 0; i < m_globals.size(); i++)
			if (m_globals[i].Name == vdata->name) {
				if (!m_isSet) {
					if (m_usePointer) m_gen.Function.GetGlobalPointer(m_globals[i].ID);
					else m_gen.Function.GetGlobal(m_globals[i].ID);
				} else
					m_gen.Function.SetGlobal(m_globals[i].ID);

				found = true;
				break;
			}

		if (m_currentFunction.size() != 0 && !found) {
			// locals
			for (int i = 0; i < m_locals[m_currentFunction].size(); i++) {
				if (m_locals[m_currentFunction][i] == vdata->name) {
					if (!m_isSet) {
						if (m_usePointer) m_gen.Function.GetLocalPointer(i);
						else m_gen.Function.GetLocal(i);
					} else
						m_gen.Function.SetLocal(i);

					found = true;
					break;
				}
			}

			// arguments
			for (int i = 0; i < m_func.size() && !found; i++) {
				if (m_func[i].Name != m_currentFunction)
					continue;

				for (int j = 0; j < m_func[i].Arguments.size(); j++) {
					if (m_func[i].Arguments[j].Name == vdata->name) {
						if (!m_isSet) {
							if (m_usePointer) m_gen.Function.GetArgumentPointer(j);
							else m_gen.Function.GetArgument(j);
						} else
							m_gen.Function.SetArgument(j);

						found = true;
						break;
					}
				}
			}
		}

		if (!found) {
			if (!m_isSet) {
				if (m_usePointer) m_gen.Function.GetGlobalPointerByName(vdata->name);
				else m_gen.Function.GetGlobalByName(vdata->name);
			}
			else
				m_gen.Function.SetGlobalByName(vdata->name);
		}
	}

	void GLSLTranslator::translateFieldOrSwizzle(glsl::astFieldOrSwizzle *expression) {
		bool temp_isSet = m_isSet, temp_usePointer = m_usePointer;
		m_isSet = false;
		m_usePointer = true;
		translateExpression(expression->operand);
		m_isSet = temp_isSet;
		m_usePointer = temp_usePointer;
		
		if (m_isSet) m_gen.Function.SetProperty(expression->name);
		else {
			if (m_usePointer) m_gen.Function.GetPropertyPointer(expression->name);
			else m_gen.Function.GetProperty(expression->name);
		}
	}

	void GLSLTranslator::translateArraySubscript(glsl::astArraySubscript *expression) {
		// first push index
		translateExpression(expression->index);

		// then array
		bool temp_isSet = m_isSet;
		m_isSet = false;
		translateExpression(expression->operand);
		m_isSet = temp_isSet;

		if (expression->operand->type == glsl::astExpression::kVariableIdentifier) {
			if (m_isSet) m_gen.Function.SetArrayElement();
			else m_gen.Function.GetArrayElement();

		}
		
		if (m_isSet)
			translateExpression(expression->operand);
	}

	void GLSLTranslator::translateFunctionCall(glsl::astFunctionCall *expression) {
		m_exportLine(expression);
		
		Function data = matchFunction(expression->name, expression->parameters);
		std::vector<GLSLTranslator::ExpressionType> paramTypes(expression->parameters.size());
		for (int i = 0; i < data.Arguments.size(); i++)
			paramTypes[i] = m_convertExprType(data.Arguments[i].Type);

		for (int i = expression->parameters.size() - 1; i >= 0; i--) {
			translateExpression(expression->parameters[i]);

			if (!data.Name.empty() && paramTypes[i] != evaluateExpressionType(expression->parameters[i]))
				generateConvert(paramTypes[i]);
		}


		m_gen.Function.CallReturn(expression->name, expression->parameters.size());
	}

	void GLSLTranslator::translateConstructorCall(glsl::astConstructorCall *expression) {
		m_exportLine(expression);
		
		for (int i = expression->parameters.size()-1; i >= 0; i--)
			translateExpression(expression->parameters[i]);

		if (expression->type->builtin) {
			std::string tName = kTypes[((glsl::astBuiltin*)expression->type)->type];
			if (isTypeActuallyStruct(tName))
				m_gen.Function.NewObjectByName(tName, expression->parameters.size());
			else {
				ag::Type btype = m_convertBaseType(tName);

				if (btype != ag::Type::Void)
					m_gen.Function.Convert(btype);
			}
		} else {
			// TODO: push new object
			printf("[DEBUG] Calling %s constructor...\n", ((glsl::astStruct*)expression->type)->name);
			m_gen.Function.NewObjectByName(((glsl::astStruct*)expression->type)->name, expression->parameters.size());
		}
	}

	void GLSLTranslator::translateDeclarationVariable(glsl::astFunctionVariable *variable) {
		m_exportLine(variable);
		
		glsl::astVariable* vdata = (glsl::astVariable*)variable;

		m_locals[m_currentFunction].push_back(vdata->name);

		if (variable->initialValue) {
			translateExpression(variable->initialValue);

			GLSLTranslator::ExpressionType lType = m_convertExprType(kTypes[((glsl::astBuiltin*)vdata->baseType)->type]);
			GLSLTranslator::ExpressionType rType = evaluateExpressionType(variable->initialValue);
			if (lType != rType)
				generateConvert(lType);

			m_gen.Function.SetLocal(m_locals[m_currentFunction].size() - 1);
		}


		if (!vdata->baseType->builtin) {
			std::string structName = ((glsl::astStruct*)vdata->baseType)->name;

			if (!variable->initialValue) {
				m_gen.Function.NewObjectByName(structName);
				m_gen.Function.SetLocal(m_locals[m_currentFunction].size() - 1);
			}

			printf("[DEBUG] Declaring variable %s with type %s\n", vdata->name, structName.c_str());
		}
		else {
			std::string structName = kTypes[((glsl::astBuiltin*)vdata->baseType)->type];

			if (isTypeActuallyStruct(structName)) {
				if (!variable->initialValue) {
					m_gen.Function.NewObjectByName(structName, 0);
					m_gen.Function.SetLocal(m_locals[m_currentFunction].size() - 1);
				}

				printf("[DEBUG] Declaring variable %s with type %s\n", vdata->name, structName.c_str());
			}
		}
	}

	void GLSLTranslator::translatePostIncrement(glsl::astPostIncrementExpression *expression) {
		m_exportLine(expression);
		translateExpression(expression->operand);

		m_usePointer = true;
		translateExpression(expression->operand);
		m_gen.Function.Increment();
		m_gen.Function.PopStack();
		m_usePointer = false;
	}

	void GLSLTranslator::translatePostDecrement(glsl::astPostDecrementExpression *expression) {
		m_exportLine(expression);
		translateExpression(expression->operand);

		m_usePointer = true;
		translateExpression(expression->operand);
		m_gen.Function.Decrement();
		m_gen.Function.PopStack();
		m_usePointer = false;
	}

	void GLSLTranslator::translateUnaryMinus(glsl::astUnaryMinusExpression *expression) {
		translateExpression(expression->operand);
		m_gen.Function.Negate();
	}

	void GLSLTranslator::translateUnaryPlus(glsl::astUnaryPlusExpression *expression) {
		// TODO?
		translateExpression(expression->operand);
	}

	void GLSLTranslator::translateUnaryBitNot(glsl::astUnaryBitNotExpression *expression) {
		translateExpression(expression->operand);
		m_gen.Function.BitNot();
	}

	void GLSLTranslator::translateUnaryLogicalNot(glsl::astUnaryLogicalNotExpression *expression) {
		translateExpression(expression->operand);
		m_gen.Function.Not();
	}

	void GLSLTranslator::translatePrefixIncrement(glsl::astPrefixIncrementExpression *expression) {
		m_exportLine(expression);
		
		m_usePointer = true;
		translateExpression(expression->operand);
		m_gen.Function.Increment();
		m_gen.Function.PopStack();
		m_usePointer = false;

		translateExpression(expression->operand);
	}

	void GLSLTranslator::translatePrefixDecrement(glsl::astPrefixDecrementExpression *expression) {
		m_exportLine(expression);
		
		m_usePointer = true;
		translateExpression(expression->operand);
		m_gen.Function.Decrement();
		m_gen.Function.PopStack();
		m_usePointer = false;

		translateExpression(expression->operand);
	}

	void GLSLTranslator::translateAssign(glsl::astAssignmentExpression *expression) {
		m_exportLine(expression);

		GLSLTranslator::ExpressionType lType = evaluateExpressionType(expression->operand1);
		GLSLTranslator::ExpressionType rType = evaluateExpressionType(expression->operand2);

		if (expression->assignment != glsl::kOperator_assign) // push the lhs to the stack too if we are using some +=, -=, etc... operator
			translateExpression(expression->operand1);
		if (rType.Columns * rType.Rows > 1 && lType.Columns * lType.Rows == 1)
			generateConvert(rType);

		translateExpression(expression->operand2); // rhs
		if (lType.Columns * lType.Rows > 1 && rType.Columns * rType.Rows == 1)
			generateConvert(lType);

		if (expression->assignment == glsl::kOperator_add_assign)
			m_gen.Function.Add();
		else if (expression->assignment == glsl::kOperator_sub_assign)
			m_gen.Function.Subtract();
		else if (expression->assignment == glsl::kOperator_multiply_assign)
			m_gen.Function.Multiply();
		else if (expression->assignment == glsl::kOperator_divide_assign)
			m_gen.Function.Divide();
		else if (expression->assignment == glsl::kOperator_modulus_assign)
			m_gen.Function.Modulo();
		else if (expression->assignment == glsl::kOperator_shift_left_assign)
			m_gen.Function.BitLeftShift();
		else if (expression->assignment == glsl::kOperator_shift_right_assign)
			m_gen.Function.BitRightShift();
		else if (expression->assignment == glsl::kOperator_bit_and_assign)
			m_gen.Function.BitAnd();
		else if (expression->assignment == glsl::kOperator_bit_xor_assign)
			m_gen.Function.BitXor();
		else if (expression->assignment == glsl::kOperator_bit_xor_assign)
			m_gen.Function.BitOr();

		GLSLTranslator::ExpressionType lhsType = evaluateExpressionType(expression->operand1);

		if (lhsType != evaluateExpressionType(expression->operand2))
			generateConvert(lhsType); // TODO: only cast when typeof(lhs) != typeof(rhs)
			
		m_isSet = true;
		translateExpression(expression->operand1); // lhs
		m_isSet = false;
	}

	Function GLSLTranslator::matchFunction(const char* name, const std::vector<glsl::astConstantExpression*>& params)
	{
		Function ret;

		int match_args = -1;

		std::vector<GLSLTranslator::ExpressionType> paramTypes(params.size());
		for (int i = 0; i < paramTypes.size(); i++)
			paramTypes[i] = evaluateExpressionType(params[i]);

		printf("[DEBUG] Trying to find a match with: ");
		for (int i = 0; i < paramTypes.size(); i++)
			printf("%s, ", paramTypes[i].Name.c_str());
		printf("\n");

		for (int i = 0; i < m_func.size(); i++) {
			if (m_func[i].Name == name) {
				if (m_func[i].Arguments.size() != params.size())
					continue;

				int func_matches = 0;

				for (int j = 0; j < m_func[i].Arguments.size(); j++) {
					// user defined structures
					if (m_convertExprType(m_func[i].Arguments[j].Type).Type == ag::Type::Object) {
						// TODO: check if types match
						if (m_func[i].Arguments[j].Type == paramTypes[j].Name)
							func_matches++;
					}
					else {
						if (paramTypes[j] == m_convertExprType(m_func[i].Arguments[j].Type))
							func_matches++;
					}
				}

				if (func_matches > match_args) {
					if (func_matches == params.size())
						return m_func[i];
					ret = m_func[i];
					match_args = func_matches;
				}
			}
		}
		printf("[DEBUG] Best match: %d\n",  match_args);
		return ret;
	}


	void GLSLTranslator::generateConvert(GLSLTranslator::ExpressionType etype)
	{
		if (etype.Type == ag::Type::Object) {
			// TODO: ?
		}
		else if (etype.Type == ag::Type::Void) { }
		else {
			if (etype.Columns * etype.Rows == 1)
				m_gen.Function.Convert(etype.Type);
			else
				m_gen.Function.NewObjectByName(etype.Name, 1); // example: ivec3(vec3())
		}
	}
	GLSLTranslator::ExpressionType GLSLTranslator::evaluateExpressionType(glsl::astExpression* expr)
	{
		switch (expr->type) {
		case glsl::astExpression::kIntConstant:
			return GLSLTranslator::ExpressionType("int", ag::Type::Int, 1, 1);
		case glsl::astExpression::kUIntConstant:
			return GLSLTranslator::ExpressionType("uint", ag::Type::UInt, 1, 1);
		case glsl::astExpression::kFloatConstant:
			return GLSLTranslator::ExpressionType("float", ag::Type::Float, 1, 1);
		case glsl::astExpression::kDoubleConstant:
			return GLSLTranslator::ExpressionType("double", ag::Type::Float, 1, 1);
		case glsl::astExpression::kBoolConstant:
			return GLSLTranslator::ExpressionType("bool", ag::Type::UChar, 1, 1);
		case glsl::astExpression::kFunctionCall: {
			glsl::astFunctionCall* func = (glsl::astFunctionCall*)expr;
			Function data = matchFunction(func->name, func->parameters);
			return m_convertExprType(data.ReturnType);
		} break;
		case glsl::astExpression::kConstructorCall: {
			glsl::astConstructorCall* constr = (glsl::astConstructorCall*)expr;

			std::string vartype = "";
			if (constr->type->builtin)
				vartype = kTypes[((glsl::astBuiltin*)constr->type)->type];
			else
				vartype = ((glsl::astStruct*)constr->type)->name;

			return m_convertExprType(vartype);
		} break;
		case glsl::astExpression::kPostIncrement: {
			glsl::astPostIncrementExpression* opdata = (glsl::astPostIncrementExpression*)expr;
			return evaluateExpressionType(opdata->operand);
		} break;
		case glsl::astExpression::kPostDecrement: {
			glsl::astPostDecrementExpression* opdata = (glsl::astPostDecrementExpression*)expr;
			return evaluateExpressionType(opdata->operand);
		} break;
		case glsl::astExpression::kUnaryMinus: {
			glsl::astUnaryMinusExpression* opdata = (glsl::astUnaryMinusExpression*)expr;
			return evaluateExpressionType(opdata->operand);
		} break;
		case glsl::astExpression::kUnaryPlus: {
			glsl::astUnaryPlusExpression* opdata = (glsl::astUnaryPlusExpression*)expr;
			return evaluateExpressionType(opdata->operand);
		} break;
		case glsl::astExpression::kBitNot: {
			glsl::astUnaryBitNotExpression* opdata = (glsl::astUnaryBitNotExpression*)expr;
			return evaluateExpressionType(opdata->operand);
		} break;
		case glsl::astExpression::kLogicalNot: {
			glsl::astUnaryLogicalNotExpression* opdata = (glsl::astUnaryLogicalNotExpression*)expr;
			return evaluateExpressionType(opdata->operand);
		} break;
		case glsl::astExpression::kPrefixIncrement: {
			glsl::astPrefixIncrementExpression* opdata = (glsl::astPrefixIncrementExpression*)expr;
			return evaluateExpressionType(opdata->operand);
		} break;
		case glsl::astExpression::kPrefixDecrement: {
			glsl::astPrefixDecrementExpression* opdata = (glsl::astPrefixDecrementExpression*)expr;
			return evaluateExpressionType(opdata->operand);
		} break;
		case glsl::astExpression::kAssign: {
			glsl::astAssignmentExpression* opdata = (glsl::astAssignmentExpression*)expr;
			return evaluateExpressionType(opdata->operand1);
		} break;
		case glsl::astExpression::kOperation: {
			glsl::astOperationExpression* opdata = (glsl::astOperationExpression*)expr;
			GLSLTranslator::ExpressionType type1 = evaluateExpressionType(opdata->operand1);
			GLSLTranslator::ExpressionType type2 = evaluateExpressionType(opdata->operand2);

			return m_mergeExprType(opdata->operation, type1, type2);
		} break;
		case glsl::astExpression::kTernary: {
			glsl::astTernaryExpression* opdata = (glsl::astTernaryExpression*)expr;
			return evaluateExpressionType(opdata->onTrue);
		} break;
		case glsl::astExpression::kVariableIdentifier: {
			glsl::astVariableIdentifier* varexpr = (glsl::astVariableIdentifier*)expr;
			std::string vartype = "";
			if (varexpr->variable->baseType->builtin)
				vartype = kTypes[((glsl::astBuiltin*)varexpr->variable->baseType)->type];
			else
				vartype = ((glsl::astStruct*)varexpr->variable->baseType)->name;

			return m_convertExprType(vartype);
		} break;
		case glsl::astExpression::kFieldOrSwizzle:
		{
			glsl::astFieldOrSwizzle* field = (glsl::astFieldOrSwizzle*)expr;
			GLSLTranslator::ExpressionType structBType = evaluateExpressionType(field->operand);

			if (structBType.Type == ag::Type::Object) {
				// user defined structure
				for (const auto& s : m_structures)
					for (const auto& m : s.Members)
						if (m.Name == field->name)
							return m_convertExprType(m.Type);
			}
			else {
				structBType.Columns = strlen(field->name); // vec3.x -> columns == 1, vec3.xy -> columns == 2, etc...
				for (int i = 0; i < structBType.Name.size(); i++)
					if (isdigit(structBType.Name[i])) {
						structBType.Name[i] = structBType.Columns + '0';
						break;
					}
				return structBType;
			}
		} break;
		case glsl::astExpression::kArraySubscript:
		{
			glsl::astArraySubscript* arr = (glsl::astArraySubscript*)expr;
			GLSLTranslator::ExpressionType operandType = evaluateExpressionType(arr->operand);

			// TODO: matrices & arrays (?)

			// vector[index]
			if (operandType.Columns * operandType.Rows > 1 && operandType.Name.find("vec") != std::string::npos)
				operandType.Columns = operandType.Rows = 1;

			return operandType;
		} break;
		}

		return GLSLTranslator::ExpressionType();
	}

	GLSLTranslator::ExpressionType GLSLTranslator::m_convertExprType(const std::string& str)
	{
		static const std::vector<std::string> floatStructs = {
			KEYWORD(mat2)
			KEYWORD(mat3)
			KEYWORD(mat4)
			KEYWORD(mat2x2)
			KEYWORD(mat2x3)
			KEYWORD(mat2x4)
			KEYWORD(mat3x2)
			KEYWORD(mat3x3)
			KEYWORD(mat3x4)
			KEYWORD(mat4x2)
			KEYWORD(mat4x3)
			KEYWORD(mat4x4)
			KEYWORD(vec2)
			KEYWORD(vec3)
			KEYWORD(vec4)
		};
		static const std::vector<std::string> doubleStructs = {
			KEYWORD(dmat2)
			KEYWORD(dmat3)
			KEYWORD(dmat4)
			KEYWORD(dmat2x2)
			KEYWORD(dmat2x3)
			KEYWORD(dmat2x4)
			KEYWORD(dmat3x2)
			KEYWORD(dmat3x3)
			KEYWORD(dmat3x4)
			KEYWORD(dmat4x2)
			KEYWORD(dmat4x3)
			KEYWORD(dmat4x4)
			KEYWORD(dvec2)
			KEYWORD(dvec3)
			KEYWORD(dvec4)
		};
		static const std::vector<std::string> intStructs = {
			KEYWORD(ivec2)
			KEYWORD(ivec3)
			KEYWORD(ivec4)
		};
		static const std::vector<std::string> boolStructs = {
			KEYWORD(bvec2)
			KEYWORD(bvec3)
			KEYWORD(bvec4)
		};
		static const std::vector<std::string> uintStructs = {
			KEYWORD(uvec2)
			KEYWORD(uvec3)
			KEYWORD(uvec4)
		};

		static const std::vector<std::pair<int, int>> floatStructSize = {
			std::make_pair(2,2),
			std::make_pair(3,3),
			std::make_pair(4,4),
			std::make_pair(2,2),
			std::make_pair(2,3),
			std::make_pair(2,4),
			std::make_pair(3,2),
			std::make_pair(3,3),
			std::make_pair(3,4),
			std::make_pair(4,2),
			std::make_pair(4,3),
			std::make_pair(4,4),
			std::make_pair(2,1),
			std::make_pair(3,1),
			std::make_pair(4,1)
		};
		static const std::vector<std::pair<int, int>> doubleStructSize = {
			std::make_pair(2,2),
			std::make_pair(3,3),
			std::make_pair(4,4),
			std::make_pair(2,2),
			std::make_pair(2,3),
			std::make_pair(2,4),
			std::make_pair(3,2),
			std::make_pair(3,3),
			std::make_pair(3,4),
			std::make_pair(4,2),
			std::make_pair(4,3),
			std::make_pair(4,4),
			std::make_pair(2,1),
			std::make_pair(3,1),
			std::make_pair(4,1)
		};
		static const std::vector<std::pair<int, int>> intStructSize = {
			std::make_pair(2,1),
			std::make_pair(3,1),
			std::make_pair(4,1)
		};
		static const std::vector<std::pair<int, int>> boolStructSize = {
			std::make_pair(2,1),
			std::make_pair(3,1),
			std::make_pair(4,1)
		};
		static const std::vector<std::pair<int, int>> uintStructSize = {
			std::make_pair(2,1),
			std::make_pair(3,1),
			std::make_pair(4,1)
		};

		// float
		if (str == "float") return ExpressionType(str, ag::Type::Float, 1, 1);
		if (std::count(floatStructs.begin(), floatStructs.end(), str) > 0) {
			for (int i = 0; i < floatStructs.size(); i++)
				if (floatStructs[i] == str)
					return ExpressionType(str, ag::Type::Float, floatStructSize[i].first, floatStructSize[i].second);
		}

		// int
		if (str == "int") return ExpressionType(str, ag::Type::Int, 1, 1);
		if (std::count(intStructs.begin(), intStructs.end(), str) > 0) {
			for (int i = 0; i < intStructs.size(); i++)
				if (intStructs[i] == str)
					return ExpressionType(str, ag::Type::Int, intStructSize[i].first, intStructSize[i].second);
		}

		// bool
		if (str == "bool") return ExpressionType(str, ag::Type::UChar, 1, 1);
		if (std::count(boolStructs.begin(), boolStructs.end(), str) > 0) {
			for (int i = 0; i < boolStructs.size(); i++)
				if (boolStructs[i] == str)
					return ExpressionType(str, ag::Type::UChar, boolStructSize[i].first, boolStructSize[i].second);
		}

		// uint
		if (str == "uint") return ExpressionType(str, ag::Type::UInt, 1, 1);
		if (std::count(uintStructs.begin(), uintStructs.end(), str) > 0) {
			for (int i = 0; i < uintStructs.size(); i++)
				if (uintStructs[i] == str)
					return ExpressionType(str, ag::Type::UInt, uintStructSize[i].first, uintStructSize[i].second);
		}

		// TODO: oops, BlueVM doesn't support doubles (?)
		if (str == "double") return ExpressionType(str, ag::Type::Float, 1, 1);
		if (std::count(doubleStructs.begin(), doubleStructs.end(), str) > 0) {
			for (int i = 0; i < doubleStructs.size(); i++)
				if (doubleStructs[i] == str)
					return ExpressionType(str, ag::Type::Float, doubleStructSize[i].first, doubleStructSize[i].second);
		}

		if (str == "void") return ExpressionType(str, ag::Type::Void, 1, 1);

		return ExpressionType(str, ag::Type::Object, 1, 1);
	}
	GLSLTranslator::ExpressionType GLSLTranslator::m_mergeExprType(int op, const GLSLTranslator::ExpressionType& left, const GLSLTranslator::ExpressionType& right)
	{
		if (left.Type == ag::Type::Object)
			return left;
		else if (right.Type == ag::Type::Object)
			return right;
		else if (right.Type == ag::Type::Void || left.Type == ag::Type::Void)
			return GLSLTranslator::ExpressionType("void", ag::Type::Void, 1, 1);
		else {
			int compLeft = left.Columns * left.Rows;
			int compRight = right.Columns * right.Rows;

			switch (op)
			{
			case glsl::kOperator_multiply: {
				if (compLeft > 1 || compRight > 1) {
					// matrix * ...
					if (left.Rows != 1) {
						// matrix * matrix
						if (right.Rows != 1) {
							ag::Type newType = m_mergeBaseType(left.Type, right.Type);

							int rows = left.Rows;
							int cols = right.Columns;

							std::string newName = "mat";
							if (rows == cols)
								newName += std::to_string(cols);
							else
								newName += std::to_string(cols) + "x" + std::to_string(rows);

							return GLSLTranslator::ExpressionType(newName, newType, cols, rows);
						}
						// matrix * vector
						else if (right.Columns != 1) {
							ag::Type newType = m_mergeBaseType(left.Type, right.Type);

							int rows = 1;
							int cols = right.Columns;

							std::string newName = "vec" + std::to_string(cols);

							if (newType == ag::Type::Int)
								newName = "i" + newName;
							else if (newType == ag::Type::UInt)
								newName = "u" + newName;
							else if (newType == ag::Type::UChar)
								newName = "b" + newName;

							return GLSLTranslator::ExpressionType(newName, newType, cols, rows);
						}
						// matrix * scalar
						else {
							ag::Type newType = m_mergeBaseType(left.Type, right.Type);

							int rows = left.Rows;
							int cols = left.Columns;

							std::string newName = "mat";
							if (rows == cols)
								newName += std::to_string(cols);
							else
								newName += std::to_string(cols) + "x" + std::to_string(rows);

							return GLSLTranslator::ExpressionType(newName, newType, cols, rows);
						}
					}
					// scalar * matrix
					else if (right.Rows != 1) {
						ag::Type newType = m_mergeBaseType(left.Type, right.Type);

						int rows = left.Rows;
						int cols = left.Columns;

						std::string newName = "mat";
						if (rows == cols)
							newName += std::to_string(cols);
						else
							newName += std::to_string(cols) + "x" + std::to_string(rows);

						return GLSLTranslator::ExpressionType(newName, newType, cols, rows);
					}
					// vector * ...
					else if (left.Columns != 1) {
						// vector * vector
						if (right.Columns != 1) {
							ag::Type newType = m_mergeBaseType(left.Type, right.Type);

							int rows = 1;
							int cols = left.Columns;

							std::string newName = "vec" + std::to_string(cols);

							if (newType == ag::Type::Int)
								newName = "i" + newName;
							else if (newType == ag::Type::UInt)
								newName = "u" + newName;
							else if (newType == ag::Type::UChar)
								newName = "b" + newName;

							return GLSLTranslator::ExpressionType(newName, newType, cols, rows);
						}
						// vector * scalar
						else {
							ag::Type newType = m_mergeBaseType(left.Type, right.Type);

							int rows = 1;
							int cols = left.Columns;

							std::string newName = "vec" + std::to_string(cols);

							if (newType == ag::Type::Int)
								newName = "i" + newName;
							else if (newType == ag::Type::UInt)
								newName = "u" + newName;
							else if (newType == ag::Type::UChar)
								newName = "b" + newName;

							return GLSLTranslator::ExpressionType(newName, newType, cols, rows);
						}
					}
					// scalar * vector
					else if (right.Columns != 1) {
						ag::Type newType = m_mergeBaseType(left.Type, right.Type);

						int rows = 1;
						int cols = right.Columns;

						std::string newName = "vec" + std::to_string(cols);

						if (newType == ag::Type::Int)
							newName = "i" + newName;
						else if (newType == ag::Type::UInt)
							newName = "u" + newName;
						else if (newType == ag::Type::UChar)
							newName = "b" + newName;

						return GLSLTranslator::ExpressionType(newName, newType, cols, rows);
					}
				}

			} break;
			case glsl::kOperator_less: return GLSLTranslator::ExpressionType("bool", ag::Type::UChar, 1, 1);
			case glsl::kOperator_greater: return GLSLTranslator::ExpressionType("bool", ag::Type::UChar, 1, 1);
			case glsl::kOperator_less_equal: return GLSLTranslator::ExpressionType("bool", ag::Type::UChar, 1, 1);
			case glsl::kOperator_greater_equal: return GLSLTranslator::ExpressionType("bool", ag::Type::UChar, 1, 1);
			case glsl::kOperator_equal: return GLSLTranslator::ExpressionType("bool", ag::Type::UChar, 1, 1);
			case glsl::kOperator_not_equal: return GLSLTranslator::ExpressionType("bool", ag::Type::UChar, 1, 1);
			default: {
				GLSLTranslator::ExpressionType big = left;
				GLSLTranslator::ExpressionType small = right;

				if (compRight > compLeft) {
					big = right;
					small = left;
				}

				ag::Type newType = m_mergeBaseType(big.Type, small.Type);
				std::string newName = big.Name;

				size_t matPos = newName.find("mat");
				size_t vecPos = newName.find("vec");

				if (matPos != std::string::npos) {
					if (matPos == 0) {}
					else if (newType == ag::Type::Float)
						newName = newName.substr(1);
				}
				else if (vecPos != std::string::npos) {
					if (vecPos == 0) {}
					else if (newType == ag::Type::Float)
						newName = newName.substr(1);
					else if (newType == ag::Type::Int)
						newName[0] = 'i';
					else if (newType == ag::Type::UInt)
						newName[0] = 'u';
					else if (newType == ag::Type::UChar)
						newName[0] = 'b';
				}
				else if (compLeft == 1 && compRight == 1) {
					// scalar
					if (newType == ag::Type::Float)
						newName = "float";
					else if (newType == ag::Type::Int)
						newName = "int";
					else if (newType == ag::Type::UInt)
						newName = "uint";
					else if (newType == ag::Type::UChar)
						newName = "bool";
				}

				return GLSLTranslator::ExpressionType(newName, newType, big.Columns, big.Rows);
			} break;
			}
		}

		return GLSLTranslator::ExpressionType("void", ag::Type::Void, 1, 1);
	}
	ag::Type GLSLTranslator::m_mergeBaseType(ag::Type type1, ag::Type type2)
	{
		if (type1 == ag::Type::Float || type2 == ag::Type::Float)
			return ag::Type::Float;
		else if (type1 == ag::Type::UInt || type2 == ag::Type::UInt)
			return ag::Type::UInt;
		else if (type1 == ag::Type::Int || type2 == ag::Type::Int)
			return ag::Type::Int;
		else if (type1 == ag::Type::UShort || type2 == ag::Type::UShort)
			return ag::Type::UShort;
		else if (type1 == ag::Type::Short || type2 == ag::Type::Short)
			return ag::Type::Short;
		else if (type1 == ag::Type::UChar || type2 == ag::Type::UChar)
			return ag::Type::UChar;
		else if (type1 == ag::Type::Char || type2 == ag::Type::Char)
			return ag::Type::Char;

		return type1;
	}

	ag::Type GLSLTranslator::m_convertBaseType(const std::string& str)
	{
		if (str == "float") return ag::Type::Float;
		if (str == "int") return ag::Type::Int;
		if (str == "bool") return ag::Type::UChar;
		if (str == "uint") return ag::Type::UInt;

		// TODO: oops, BlueVM doesn't support doubles (?)
		if (str == "double") return ag::Type::Float;

		return ag::Type::Void;
	}


	void GLSLTranslator::translateSequence(glsl::astSequenceExpression *expression) {
		m_exportLine(expression);
		
		translateExpression(expression->operand1);
		translateExpression(expression->operand2);
	}

	void GLSLTranslator::translateOperation(glsl::astOperationExpression *expression) {
		GLSLTranslator::ExpressionType lType = evaluateExpressionType(expression->operand1);
		GLSLTranslator::ExpressionType rType = evaluateExpressionType(expression->operand2);

		translateExpression(expression->operand1);
		if (rType.Columns * rType.Rows > 1 && lType.Columns * lType.Rows == 1)
			generateConvert(rType);

		translateExpression(expression->operand2);
		if (lType.Columns * lType.Rows > 1 && rType.Columns * rType.Rows == 1)
			generateConvert(lType);

		translateOperator(expression->operation);
	}

	void GLSLTranslator::translateTernary(glsl::astTernaryExpression *expression) {
		m_exportLine(expression);
		
		translateExpression(expression->condition);
		size_t pos = m_gen.Function.If();
		// ?
		translateExpression(expression->onTrue);
		size_t goto_skip = m_gen.Function.Goto();
		// :
		m_gen.Function.SetAddress(pos, m_gen.Function.GetCurrentAddress());
		translateExpression(expression->onFalse);

		m_gen.Function.SetAddress(goto_skip, m_gen.Function.GetCurrentAddress());
	}

	void GLSLTranslator::translateExpression(glsl::astExpression *expression) {
		switch (expression->type) {
		case glsl::astExpression::kIntConstant:
			return translateIntConstant((glsl::astIntConstant*)expression);
		case glsl::astExpression::kUIntConstant:
			return translateUIntConstant((glsl::astUIntConstant*)expression);
		case glsl::astExpression::kFloatConstant:
			return translateFloatConstant((glsl::astFloatConstant*)expression);
		case glsl::astExpression::kDoubleConstant:
			return translateDoubleConstant((glsl::astDoubleConstant*)expression);
		case glsl::astExpression::kBoolConstant:
			return translateBoolConstant((glsl::astBoolConstant*)expression);
		case glsl::astExpression::kVariableIdentifier:
			return translateVariableIdentifier((glsl::astVariableIdentifier*)expression);
		case glsl::astExpression::kFieldOrSwizzle:
			return translateFieldOrSwizzle((glsl::astFieldOrSwizzle*)expression);
		case glsl::astExpression::kArraySubscript:
			return translateArraySubscript((glsl::astArraySubscript*)expression);
		case glsl::astExpression::kFunctionCall:
			return translateFunctionCall((glsl::astFunctionCall*)expression);
		case glsl::astExpression::kConstructorCall:
			return translateConstructorCall((glsl::astConstructorCall*)expression);
		case glsl::astExpression::kPostIncrement:
			return translatePostIncrement((glsl::astPostIncrementExpression*)expression);
		case glsl::astExpression::kPostDecrement:
			return translatePostDecrement((glsl::astPostDecrementExpression*)expression);
		case glsl::astExpression::kUnaryMinus:
			return translateUnaryMinus((glsl::astUnaryMinusExpression*)expression);
		case glsl::astExpression::kUnaryPlus:
			return translateUnaryPlus((glsl::astUnaryPlusExpression*)expression);
		case glsl::astExpression::kBitNot:
			return translateUnaryBitNot((glsl::astUnaryBitNotExpression*)expression);
		case glsl::astExpression::kLogicalNot:
			return translateUnaryLogicalNot((glsl::astUnaryLogicalNotExpression*)expression);
		case glsl::astExpression::kPrefixIncrement:
			return translatePrefixIncrement((glsl::astPrefixIncrementExpression*)expression);
		case glsl::astExpression::kPrefixDecrement:
			return translatePrefixDecrement((glsl::astPrefixDecrementExpression*)expression);
		case glsl::astExpression::kAssign:
			return translateAssign((glsl::astAssignmentExpression*)expression);
		case glsl::astExpression::kSequence:
			return translateSequence((glsl::astSequenceExpression*)expression);
		case glsl::astExpression::kOperation:
			return translateOperation((glsl::astOperationExpression*)expression);
		case glsl::astExpression::kTernary:
			return translateTernary((glsl::astTernaryExpression*)expression);
		}
	}

	void GLSLTranslator::translateCompoundStatement(glsl::astCompoundStatement *statement) {
		// m_gen.Function.ScopeStart(); // TODO: it doesn't work with these?
		m_exportLine(statement);
		
		printf("[DEBUG] Entering compoung statement!\n");

		for (size_t i = 0; i < statement->statements.size(); i++)
			translateStatement(statement->statements[i]);
		
		// m_gen.Function.ScopeEnd();
	}

	void GLSLTranslator::translateDeclarationStatement(glsl::astDeclarationStatement *statement) {
		for (size_t i = 0; i < statement->variables.size(); i++)
			translateDeclarationVariable(statement->variables[i]);
	}

	void GLSLTranslator::translateExpressionStatement(glsl::astExpressionStatement *statement) {
		translateExpression(statement->expression);
	}

	void GLSLTranslator::translateIfStetement(glsl::astIfStatement *statement) {
		m_exportLine(statement);
		
		translateExpression(statement->condition);
		size_t pos = m_gen.Function.If();
		translateStatement(statement->thenStatement);
		m_gen.Function.SetAddress(pos, m_gen.Function.GetCurrentAddress());

		if (statement->elseStatement)
			translateStatement(statement->elseStatement);
	}

	void GLSLTranslator::translateSwitchStatement(glsl::astSwitchStatement *statement) {
		m_exportLine(statement);
		
		m_breaks.push(std::vector<size_t>());

		glsl::vector<glsl::astStatement*>& stats = statement->statements;
		for (size_t i = 0; i < stats.size(); i++) {
			translateExpression(statement->expression);

			if (stats[i]->type == glsl::astStatement::kCaseLabel) {
				for (size_t j = i; j < stats.size(); j++) {
					if (j != i && stats[j]->type == glsl::astStatement::kCaseLabel) {
						i = j-1;
						break;
					}
					translateStatement(stats[j]);
				}
			} else continue;

			if (!m_caseIfDefault)
				m_gen.Function.SetAddress(m_caseIfAddr, m_gen.Function.GetCurrentAddress());
			else
				m_gen.Function.PopStack();
		}

		for (size_t i = 0; i < m_breaks.top().size(); i++)
			m_gen.Function.SetAddress(m_breaks.top()[i], m_gen.Function.GetCurrentAddress());

		m_breaks.pop();
	}

	void GLSLTranslator::translateCaseLabelStatement(glsl::astCaseLabelStatement *statement) {
		m_exportLine(statement);
		
		m_caseIfDefault = statement->isDefault;
		if (statement->isDefault) { }
		else {
			translateExpression(statement->condition);
			m_gen.Function.Equal();
			m_caseIfAddr = m_gen.Function.If();
		}
	}

	void GLSLTranslator::translateWhileStatement(glsl::astWhileStatement *statement) {
		m_exportLine(statement);
		
		size_t rewind_adr = m_gen.Function.GetCurrentAddress();

		m_breaks.push(std::vector<size_t>());
		m_continueAddr.push(rewind_adr);

		switch (statement->condition->type) {
		case glsl::astStatement::kDeclaration:
			translateDeclarationStatement((glsl::astDeclarationStatement*)statement->condition);
			break;
		case glsl::astStatement::kExpression:
			translateExpressionStatement((glsl::astExpressionStatement*)statement->condition);
			break;
		}
		size_t while_if_ref = m_gen.Function.If();

		translateStatement(statement->body);
		size_t rewind_ref = m_gen.Function.Goto();

		// goto points to the if statement, if statement points to the command after goto
		m_gen.Function.SetAddress(rewind_ref, rewind_adr);
		m_gen.Function.SetAddress(while_if_ref, m_gen.Function.GetCurrentAddress());
		
		for (size_t i = 0; i < m_breaks.top().size(); i++)
			m_gen.Function.SetAddress(m_breaks.top()[i], m_gen.Function.GetCurrentAddress());

		m_continueAddr.pop();
		m_breaks.pop();
	}

	void GLSLTranslator::translateDoStatement(glsl::astDoStatement *statement) {
		m_exportLine(statement);
		
		size_t rewind_adr = m_gen.Function.GetCurrentAddress();

		m_breaks.push(std::vector<size_t>());
		m_continueAddr.push(rewind_adr);

		translateStatement(statement->body);

		translateExpression(statement->condition);
		m_gen.Function.Not();

		size_t rewind_ref = m_gen.Function.If();
		m_gen.Function.SetAddress(rewind_ref, rewind_adr);

		for (size_t i = 0; i < m_breaks.top().size(); i++)
			m_gen.Function.SetAddress(m_breaks.top()[i], m_gen.Function.GetCurrentAddress());

		m_continueAddr.pop();
		m_breaks.pop();
	}

	void GLSLTranslator::translateForStatement(glsl::astForStatement *statement) {
		m_breaks.push(std::vector<size_t>());

		if (statement->init) {
			switch (statement->init->type) {
			case glsl::astStatement::kDeclaration:
				translateDeclarationStatement((glsl::astDeclarationStatement*)statement->init);
				break;
			case glsl::astStatement::kExpression:
				translateExpressionStatement((glsl::astExpressionStatement*)statement->init);
				break;
			}
		}
		int first_test = -1;
		int reg_test = -1;
		int rewind_adr = -1;
		
		if (statement->condition) {
			translateExpression(statement->condition);
			first_test = m_gen.Function.If();
		}
		
		// on first iteration skip update() and regular condition()
		size_t body_start = m_gen.Function.Goto();
		m_continueAddr.push(m_gen.Function.GetCurrentAddress());

		m_exportLine(statement, true);
		
		// the third part of for loop
		if (statement->loop)
			translateExpression(statement->loop);

		if (statement->condition) {
			translateExpression(statement->condition);
			reg_test = m_gen.Function.If();
		}

		m_gen.Function.SetAddress(body_start, m_gen.Function.GetCurrentAddress());
		translateStatement(statement->body);

		// go to top (m_continueAddr == top) where top is where loop -> condition -> body starts
		size_t rewind_ref = m_gen.Function.Goto();
		m_gen.Function.SetAddress(rewind_ref, m_continueAddr.top());

		// address to which vm should go if condition is false
		if (reg_test != -1)
			m_gen.Function.SetAddress(reg_test, m_gen.Function.GetCurrentAddress());
		if (first_test != -1)
			m_gen.Function.SetAddress(first_test, m_gen.Function.GetCurrentAddress());

		for (size_t i = 0; i < m_breaks.top().size(); i++)
			m_gen.Function.SetAddress(m_breaks.top()[i], m_gen.Function.GetCurrentAddress());

		m_continueAddr.pop();
		m_breaks.pop();
	}

	void GLSLTranslator::translateContinueStatement() {
		m_gen.Function.SetAddress(m_gen.Function.Goto(), m_continueAddr.top());
	}

	void GLSLTranslator::translateBreakStatement() {
		if (m_breaks.size() > 0)
			m_breaks.top().push_back(m_gen.Function.Goto());
	}

	void GLSLTranslator::translateReturnStatement(glsl::astReturnStatement *statement) {
		m_exportLine(statement);
		
		if (statement->expression) {
			translateExpression(statement->expression);

			GLSLTranslator::ExpressionType rType = m_convertExprType(m_curFuncData->ReturnType);
			generateConvert(rType);
		}

		m_gen.Function.Return();
	}

	void GLSLTranslator::translateDiscardStatement() {
		m_gen.Function.Call("$$discard", 0);
		m_gen.Function.Return();
	}

	void GLSLTranslator::translateStatement(glsl::astStatement *statement) {
		switch (statement->type) {
		case glsl::astStatement::kCompound:
			return translateCompoundStatement((glsl::astCompoundStatement*)statement);
		case glsl::astStatement::kDeclaration:
			return translateDeclarationStatement((glsl::astDeclarationStatement*)statement);
		case glsl::astStatement::kExpression:
			return translateExpressionStatement((glsl::astExpressionStatement*)statement);
		case glsl::astStatement::kIf:
			return translateIfStetement((glsl::astIfStatement*)statement);
		case glsl::astStatement::kSwitch:
			return translateSwitchStatement((glsl::astSwitchStatement*)statement);
		case glsl::astStatement::kCaseLabel:
			m_exportLine(statement);
			return translateCaseLabelStatement((glsl::astCaseLabelStatement*)statement);
		case glsl::astStatement::kWhile:
			return translateWhileStatement((glsl::astWhileStatement*)statement);
		case glsl::astStatement::kDo:
			return translateDoStatement((glsl::astDoStatement*)statement);
		case glsl::astStatement::kFor:
			return translateForStatement((glsl::astForStatement*)statement);
		case glsl::astStatement::kContinue:
			m_exportLine(statement);
			return translateContinueStatement();
		case glsl::astStatement::kBreak:
			m_exportLine(statement);
			return translateBreakStatement();
		case glsl::astStatement::kReturn:
			return translateReturnStatement((glsl::astReturnStatement*)statement);
		case glsl::astStatement::kDiscard:
			m_exportLine(statement);
			return translateDiscardStatement();
		}
	}

	void GLSLTranslator::translateFunction(glsl::astFunction *function) {
		if (function->isPrototype)
			return; // only generate bytecode for "real deals"?
		
		m_func.push_back(Function());
		Function& func = m_func[m_func.size()-1];
		
		func.Arguments.clear();

		std::vector<ag::Type> argTypes;
		std::vector<std::string> argNames;
		for (size_t i = 0; i < function->parameters.size(); i++) {
			glsl::astFunctionParameter* param = function->parameters[i];

			Variable pdata;
			pdata.ID = i;
			pdata.Name = param->name;
			if (param->baseType->builtin)
				pdata.Type = kTypes[((glsl::astBuiltin*)param->baseType)->type];
			else
				pdata.Type = ((glsl::astStruct*)param->baseType)->name;

			ag::Type btype = m_convertBaseType(pdata.Type);
			if (btype == ag::Type::Void)
				btype = ag::Type::Object;

			argTypes.push_back(btype);
			argNames.push_back(pdata.Type);

			// TODO: arrays?
			// param->isArray

			func.Arguments.push_back(pdata);
		}

		func.ID = m_gen.Function.Create(function->name, function->parameters.size(), argTypes, argNames);
		func.Name = m_currentFunction = function->name;
		
		if (function->returnType->builtin)
			func.ReturnType = kTypes[((glsl::astBuiltin*)function->returnType)->type];
		else
			func.ReturnType = ((glsl::astStruct*)function->returnType)->name;

		m_curFuncData = &func;

		m_gen.Function.SetCurrent(func.ID);

		// initialize global variables at the start of main()
		if (func.Name == m_entryFunction) {
			// create global objects (vec3, etc..)
			for (auto& gInitClass : m_initObjsInMain) {
				size_t varID = 0;
				for (auto& gVar : m_globals)
					if (gVar.Name == gInitClass.first)
						varID = gVar.ID;
				m_gen.Function.GetGlobal(varID);
				m_gen.Function.PushNull();
				m_gen.Function.Equal();
				size_t ifPos = m_gen.Function.If(); // if global == null then initialize
					m_gen.Function.NewObjectByName(gInitClass.second, 0);
					m_gen.Function.SetGlobal(varID);
				m_gen.Function.SetAddress(ifPos, m_gen.Function.GetCurrentAddress());
			}

			// init array
			for (auto& gArray : m_initArraysInMain) {
				if (gArray.second.size() > 0) {
					translateArraySize(gArray.second);
					m_gen.Function.NewArray(gArray.second.size());

					size_t varID = 0;
					for (auto& gVar : m_globals)
						if (gVar.Name == gArray.first)
							varID = gVar.ID;
					m_gen.Function.SetGlobal(varID);
				}
			}

			// init in main
			for (auto& gInit : m_initInMain) {
				if (gInit.second != nullptr) {
					translateExpression(gInit.second);
					
					size_t varID = 0;
					for (auto& gVar : m_globals)
						if (gVar.Name == gInit.first)
							varID = gVar.ID;
					m_gen.Function.SetGlobal(varID);
				}
			}
		}

		m_exportLine(function);

		// Return type not needed? function->returnType

		for (size_t i = 0; i < function->statements.size(); i++)
			translateStatement(function->statements[i]);
	}

	void GLSLTranslator::translateStructure(glsl::astStruct *structure) {
		m_structures.push_back(Structure());
		Structure& str = m_structures[m_structures.size()-1];

		if (structure->name) {
			str.Name = structure->name;
			str.ID = m_gen.AddObject(structure->name);
		}
		for (size_t i = 0; i < structure->fields.size(); i++) {
			glsl::astVariable* prop = structure->fields[i];
			Variable propData;

			propData.Name = prop->name;
			m_gen.AddProperty(str.Name, prop->name);
			
			if (prop->baseType->builtin)
				propData.Type = kTypes[((glsl::astBuiltin*)prop->baseType)->type];
			else
				propData.Type = ((glsl::astStruct*)prop->baseType)->name;

			str.Members.push_back(propData);
				
			// TODO: array members
			// if (variable->isArray)
			// 	printArraySize(variable->arraySizes);
		}
	}

	void GLSLTranslator::translate(glsl::astTU *tu) {
		for (size_t i = 0; i < tu->structures.size(); i++)
			translateStructure(tu->structures[i]);
		for (size_t i = 0; i < tu->globals.size(); i++)
			translateGlobalVariable(tu->globals[i]);
		for (size_t i = 0; i < tu->functions.size(); i++)
			translateFunction(tu->functions[i]);
	}
}