//=============================================================================
//
// Render/HLSLGenerator.cpp
//
// Created by Max McGuire (max@unknownworlds.com)
// Copyright (c) 2013, Unknown Worlds Entertainment, Inc.
//
//=============================================================================

#include <hlslparser/Engine.h>
#include <hlslparser/HLSLParser.h>
#include <hlslparser/HLSLTree.h>

#include <ShaderDebugger/Utils.h>
#include <ShaderDebugger/HLSLLibrary.h>
#include <ShaderDebugger/HLSLCompiler.h>
#include <ShaderDebugger/CommonLibrary.h>

#include <wgtcc/cpp.h>
#include <sstream>

namespace sd
{
	HLSLCompiler::ExpressionType HLSLCompiler::m_convertType(const M4::HLSLBaseType& type, const char* userDefined)
	{
		switch (type)
		{
		case M4::HLSLBaseType_Void:         return ExpressionType("void", ag::Type::Void);
		case M4::HLSLBaseType_Float:        return ExpressionType("float", ag::Type::Float);
		case M4::HLSLBaseType_Half:         return ExpressionType("half", ag::Type::Float);
		case M4::HLSLBaseType_Bool:         return ExpressionType("bool", ag::Type::UChar);
		case M4::HLSLBaseType_Int:          return ExpressionType("int", ag::Type::Int);
		case M4::HLSLBaseType_Uint:         return ExpressionType("uint", ag::Type::UInt);
		case M4::HLSLBaseType_Float2:       return ExpressionType("float2", ag::Type::Float, 1, 2);
		case M4::HLSLBaseType_Float3:       return ExpressionType("float3", ag::Type::Float, 1, 3);
		case M4::HLSLBaseType_Float4:       return ExpressionType("float4", ag::Type::Float, 1, 4);
		case M4::HLSLBaseType_Float2x2:     return ExpressionType("float2x2", ag::Type::Float, 2, 2);
		case M4::HLSLBaseType_Float3x3:     return ExpressionType("float3x3", ag::Type::Float, 3, 3);
		case M4::HLSLBaseType_Float4x4:     return ExpressionType("float4x4", ag::Type::Float, 4, 4);
		case M4::HLSLBaseType_Float4x3:     return ExpressionType("float4x3", ag::Type::Float, 3, 4);
		case M4::HLSLBaseType_Float4x2:     return ExpressionType("float4x2", ag::Type::Float, 2, 4);
		case M4::HLSLBaseType_Half2:       return ExpressionType("float2", ag::Type::Float, 1, 2);
		case M4::HLSLBaseType_Half3:       return ExpressionType("float3", ag::Type::Float, 1, 3);
		case M4::HLSLBaseType_Half4:       return ExpressionType("float4", ag::Type::Float, 1, 4);
		case M4::HLSLBaseType_Half2x2:     return ExpressionType("float2x2", ag::Type::Float, 2, 2);
		case M4::HLSLBaseType_Half3x3:     return ExpressionType("float3x3", ag::Type::Float, 3, 3);
		case M4::HLSLBaseType_Half4x4:     return ExpressionType("float4x4", ag::Type::Float, 4, 4);
		case M4::HLSLBaseType_Half4x3:     return ExpressionType("float4x3", ag::Type::Float, 3, 4);
		case M4::HLSLBaseType_Half4x2:     return ExpressionType("float4x2", ag::Type::Float, 2, 4);
		case M4::HLSLBaseType_Bool2:        return ExpressionType("bool2", ag::Type::UChar, 1, 2);
		case M4::HLSLBaseType_Bool3:        return ExpressionType("bool3", ag::Type::UChar, 1, 3);
		case M4::HLSLBaseType_Bool4:        return ExpressionType("bool4", ag::Type::UChar, 1, 4);
		case M4::HLSLBaseType_Int2:         return ExpressionType("int2", ag::Type::Int, 1, 2);
		case M4::HLSLBaseType_Int3:         return ExpressionType("int3", ag::Type::Int, 1, 3);
		case M4::HLSLBaseType_Int4:         return ExpressionType("int4", ag::Type::Int, 1, 4);
		case M4::HLSLBaseType_Uint2:         return ExpressionType("uint2", ag::Type::UInt, 1, 2);
		case M4::HLSLBaseType_Uint3:         return ExpressionType("uint3", ag::Type::UInt, 1, 3);
		case M4::HLSLBaseType_Uint4:         return ExpressionType("uint4", ag::Type::UInt, 1, 4);
		case M4::HLSLBaseType_Texture1D:    return ExpressionType("Texture1D", ag::Type::Object);
		case M4::HLSLBaseType_Texture2D:    return ExpressionType("Texture2D", ag::Type::Object);
		case M4::HLSLBaseType_Texture3D:    return ExpressionType("Texture3D", ag::Type::Object);
		case M4::HLSLBaseType_TextureCube:  return ExpressionType("TextureCube", ag::Type::Object);
		case M4::HLSLBaseType_TextureCubeArray: return ExpressionType("TextureCubeArray", ag::Type::Object);
		case M4::HLSLBaseType_Texture2DMS:      return ExpressionType("Texture2DMS", ag::Type::Object);
		case M4::HLSLBaseType_Texture1DArray:   return ExpressionType("Texture1DArray", ag::Type::Object);
		case M4::HLSLBaseType_Texture2DArray:   return ExpressionType("Texture2DArray", ag::Type::Object);
		case M4::HLSLBaseType_Texture2DMSArray: return ExpressionType("Texture2DMSArray", ag::Type::Object);
		case M4::HLSLBaseType_RWTexture1D: return ExpressionType("RWTexture1D", ag::Type::Object);
		case M4::HLSLBaseType_RWTexture2D: return ExpressionType("RWTexture2D", ag::Type::Object);
		case M4::HLSLBaseType_RWTexture3D: return ExpressionType("RWTexture3D", ag::Type::Object);
		case M4::HLSLBaseType_SamplerState:    return ExpressionType("SamplerState", ag::Type::Object);
		case M4::HLSLBaseType_UserDefined:      return ExpressionType(userDefined, ag::Type::Object);
		default: return ExpressionType("void", ag::Type::Void);
		}
	}
	HLSLCompiler::ExpressionType HLSLCompiler::m_convertType(const M4::HLSLType& type)
	{
		return m_convertType(type.baseType, type.typeName);
	}
	HLSLCompiler::ExpressionType HLSLCompiler::m_convertType(const std::string& type)
	{
		if (type == "void") return m_convertType(M4::HLSLBaseType_Void);
		else if (type == "float") return m_convertType(M4::HLSLBaseType_Float);
		else if (type == "half") return m_convertType(M4::HLSLBaseType_Half);
		else if (type == "bool") return m_convertType(M4::HLSLBaseType_Bool);
		else if (type == "int") return m_convertType(M4::HLSLBaseType_Int);
		else if (type == "uint") return m_convertType(M4::HLSLBaseType_Uint);
		else if (type == "float2") return m_convertType(M4::HLSLBaseType_Float2);
		else if (type == "float3") return m_convertType(M4::HLSLBaseType_Float3);
		else if (type == "float4") return m_convertType(M4::HLSLBaseType_Float4);
		else if (type == "float2x2") return m_convertType(M4::HLSLBaseType_Float2x2);
		else if (type == "float3x3") return m_convertType(M4::HLSLBaseType_Float3x3);
		else if (type == "float4x4") return m_convertType(M4::HLSLBaseType_Float4x4);
		else if (type == "float4x3") return m_convertType(M4::HLSLBaseType_Float4x3);
		else if (type == "float4x2") return m_convertType(M4::HLSLBaseType_Float4x2);
		else if (type == "half2") return m_convertType(M4::HLSLBaseType_Half2);
		else if (type == "half3") return m_convertType(M4::HLSLBaseType_Half3);
		else if (type == "half4") return m_convertType(M4::HLSLBaseType_Half4);
		else if (type == "half2x2") return m_convertType(M4::HLSLBaseType_Half2x2);
		else if (type == "half3x3") return m_convertType(M4::HLSLBaseType_Half3x3);
		else if (type == "half4x4") return m_convertType(M4::HLSLBaseType_Half4x4);
		else if (type == "half4x3") return m_convertType(M4::HLSLBaseType_Half4x3);
		else if (type == "half4x2") return m_convertType(M4::HLSLBaseType_Half4x2);
		else if (type == "bool2") return m_convertType(M4::HLSLBaseType_Bool2);
		else if (type == "bool3") return m_convertType(M4::HLSLBaseType_Bool3);
		else if (type == "bool4") return m_convertType(M4::HLSLBaseType_Bool4);
		else if (type == "int2") return m_convertType(M4::HLSLBaseType_Int2);
		else if (type == "int3") return m_convertType(M4::HLSLBaseType_Int3);
		else if (type == "int4") return m_convertType(M4::HLSLBaseType_Int4);
		else if (type == "uint2") return m_convertType(M4::HLSLBaseType_Uint2);
		else if (type == "uint3") return m_convertType(M4::HLSLBaseType_Uint3);
		else if (type == "uint4") return m_convertType(M4::HLSLBaseType_Uint4);
		else if (type == "Texture1D") return m_convertType(M4::HLSLBaseType_Texture1D);
		else if (type == "Texture2D") return m_convertType(M4::HLSLBaseType_Texture2D);
		else if (type == "Texture3D") return m_convertType(M4::HLSLBaseType_Texture3D);
		else if (type == "TextureCube") return m_convertType(M4::HLSLBaseType_TextureCube);
		else if (type == "TextureCubeArray") return m_convertType(M4::HLSLBaseType_TextureCubeArray);
		else if (type == "Texture2DMS") return m_convertType(M4::HLSLBaseType_Texture2DMS);
		else if (type == "Texture1DArray") return m_convertType(M4::HLSLBaseType_Texture1DArray);
		else if (type == "Texture2DArray") return m_convertType(M4::HLSLBaseType_Texture2DArray);
		else if (type == "Texture2DMSArray") return m_convertType(M4::HLSLBaseType_Texture2DMSArray);
		else if (type == "RWTexture1D") return m_convertType(M4::HLSLBaseType_RWTexture1D);
		else if (type == "RWTexture2D") return m_convertType(M4::HLSLBaseType_RWTexture2D);
		else if (type == "RWTexture3D") return m_convertType(M4::HLSLBaseType_RWTexture3D);
		else if (type == "SamplerState" || type == "sampler") return m_convertType(M4::HLSLBaseType_SamplerState);
		return m_convertType(M4::HLSLBaseType_UserDefined, type.c_str());
	}
	bool HLSLCompiler::m_isTypeActuallyStruct(HLSLCompiler::ExpressionType type)
	{
		return type.Type == ag::Type::Object || (type.Rows * type.Columns != 1);
	}

	char* stralloc(const std::string& str)
	{
		char* ret = (char*)calloc(str.size() + 1, sizeof(char));
		strcpy(ret, str.c_str());
		return ret;
	}

	Function HLSLCompiler::m_matchFunction(const char* name, int argCount, M4::HLSLExpression* args)
	{
		Function ret;
		ret.Name = "";

		int match_args = -1;

		int aIndex = 0;
		std::vector<HLSLCompiler::ExpressionType> paramTypes(argCount);
		M4::HLSLExpression* arg = args;
		while (arg) {
			paramTypes[aIndex] = m_convertType(arg->expressionType);

			aIndex++;
			arg = arg->nextExpression;
		}

		//printf("[DEBUG] Trying to find a match with: ");
		//for (int i = 0; i < paramTypes.size(); i++)
		//	printf("%s, ", paramTypes[i].Name.c_str());
		//printf("\n");

		for (int i = 0; i < m_func.size(); i++) {
			if (m_func[i].Name == name) {
				if (m_func[i].Arguments.size() != argCount)
					continue;

				int func_matches = 0;

				for (int j = 0; j < m_func[i].Arguments.size(); j++) {
					// user defined structures
					if (m_convertType(m_func[i].Arguments[j].Type).Type == ag::Type::Object) {
						// TODO: check if types match
						if (m_func[i].Arguments[j].Type == paramTypes[j].Name)
							func_matches++;
					}
					else {
						if (paramTypes[j] == m_convertType(m_func[i].Arguments[j].Type))
							func_matches++;
					}
				}

				if (func_matches > match_args) {
					if (func_matches == argCount)
						return m_func[i];
					ret = m_func[i];
					match_args = func_matches;
				}
			}
		}
		//printf("[DEBUG] Best match: %d\n",  match_args);
		return ret;
	}

	void HLSLCompiler::m_freeImmediate()
	{
		for (int i = 0; i < m_immFuncs.size(); i++) {
			M4::HLSLArgument* arg = m_immFuncs[i]->argument;

			while (arg) {
				M4::HLSLArgument* curArg = arg;
				arg = arg->nextArgument;
				delete curArg;
			}

			delete m_immFuncs[i];
		}
		for (int i = 0; i < m_immStructs.size(); i++) {
			M4::HLSLStructField* field = m_immStructs[i]->field;

			while (field) {
				M4::HLSLStructField* curArg = field;
				field = field->nextField;
				delete curArg;
			}

			delete m_immStructs[i];
		}
		m_immStructs.clear();
		m_immFuncs.clear();
	}

	// hlslparser allocator functions
	void* New(void* userData, size_t size)
	{
		(void)userData;
		return malloc(size);
	}
	void* NewArray(void* userData, size_t size, size_t count)
	{
		(void)userData;
		return malloc(size * count);
	}
	void Delete(void* userData, void* ptr)
	{
		(void)userData;
		free(ptr);
	}
	void* Realloc(void* userData, void* ptr, size_t size, size_t count)
	{
		(void)userData;
		return realloc(ptr, size * count);
	}

	// hlslparser logger functions
	// TODO: global var, change this
	static std::string hlslCompilerLastErrorMsg = "";
	void LogErrorArgs(void* userData, const char* format, va_list args)
	{
		char buffer[256];
		sprintf(buffer, format, args);

		hlslCompilerLastErrorMsg = buffer;
	}
	void LogError(void* userData, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		LogErrorArgs(userData, format, args);
		va_end(args);
	}

	HLSLCompiler::HLSLCompiler()
	{
		m_language = Compiler::Language::HLSL;
		m_isInsideBuffer = false;
		m_currentFunction = "";
		m_entryFunction = "";
		m_isSet = m_usePointer = false;
		m_caseIfDefault = false;
		m_caseIfAddr = 0;
	}
	HLSLCompiler::~HLSLCompiler()
	{
	}


	void HLSLCompiler::m_buildFuncArgPtrs()
	{
		m_builtInFuncsPtrs.clear();

		m_builtInFuncsPtrs["modf"].push_back(1); // second argument is out
		m_builtInFuncsPtrs["frexp"].push_back(1); // second argument is out
		m_builtInFuncsPtrs["sincos"].push_back(1); // 2nd argument is out
		m_builtInFuncsPtrs["sincos"].push_back(2); // 3rd argument is out
	}

	bool HLSLCompiler::Parse(ShaderType type, const std::string& source, std::string entry)
	{
		if (!m_isImmediate)
			ClearDefinitions();
		m_gen.Reset();

		for (const auto& str : m_stringTable)
			m_gen.AddString(str);

		PropertyGetter = HLSL::Swizzle;
		ObjectConstructor = Common::DefaultConstructor;

		m_lastLineSaved = -1;
		m_isSet = false;
		m_usePointer = false;
		m_caseIfDefault = false;
		m_caseIfAddr = 0;
		m_entryFunction = entry;
		m_isInsideBuffer = false;
		m_curFuncData = nullptr;
		m_currentFunction = "";
		m_gen.SetHeader(0, 2);

		m_buildFuncArgPtrs();

		std::string actualSource = source;
		if (m_isImmediate)
			actualSource = "void immediate()\n{\nreturn " + source + ";\n}"; // the return type doesn't matter here :P

		// add the user defined macros
		pp::MacroMap macroCopy = m_macros;
		pp::Preprocessor preprocess;
		for (auto& pair : m_macros)
			preprocess.AddMacro(pair.first, pair.second);
		if (!m_isImmediate) m_macros.clear();

		// preprocess
		std::stringstream sourcePreprocessed;
		pp::TokenSequence tokSeq;
		preprocess.Process(actualSource, "shader.hlsl", tokSeq);
		tokSeq.Print(sourcePreprocessed);
		actualSource = sourcePreprocessed.str();
		
		// add back the user defined macros to the macro list
		if (!m_isImmediate) {
			m_macros = preprocess.GetMacroList();
			for (auto& pair : macroCopy)
				if (m_macros.count(pair.first) == 0)
					m_macros.insert(pair);
		}

		// create allocator and logger
		M4::Allocator allocator;
		allocator.m_userData = NULL;
		allocator.New = New;
		allocator.NewArray = NewArray;
		allocator.Delete = Delete;
		allocator.Realloc = Realloc;
		M4::Logger logger;
		logger.m_userData = NULL;
		logger.LogError = LogError;
		logger.LogErrorArgList = LogErrorArgs;
		
		// create parser
		M4::HLSLParser parser(&allocator, &logger, "memory", actualSource.data(), actualSource.size());
		M4::HLSLTree tree(&allocator);
		
		// structure definitions
		for (const auto& glob : m_immGlobals) {
			M4::HLSLType globType(glob.second.first);
			if (globType.baseType == M4::HLSLBaseType_UserDefined)
				globType.typeName = glob.second.second.c_str();
			
			parser.DeclareVariable(glob.first.c_str(), globType);
		}
		
		std::vector<char*> string_pool; // <- because of the hlslparser....
		if (m_isImmediate) {
			// function definitions
			for (const auto& f : m_func) {
				M4::HLSLFunction* function = new M4::HLSLFunction();
				string_pool.push_back(stralloc(f.Name));
				function->name = string_pool[string_pool.size()-1];
				function->returnType.baseType = M4::HLSLParser::GetTypeFromString(f.ReturnType);
				function->returnType.typeName = f.ReturnType.c_str();
				function->numArguments = f.Arguments.size();

				M4::HLSLArgument* argDef = nullptr;
				size_t curArgIndex = 0;
				for (const auto& arg : f.Arguments) {
					if (!argDef)
						argDef = new M4::HLSLArgument();
					string_pool.push_back(stralloc(arg.Name));
					argDef->name = string_pool[string_pool.size() - 1];
					argDef->type.baseType = M4::HLSLParser::GetTypeFromString(arg.Type);
					argDef->type.typeName = arg.Type.c_str();

					if (function->argument == nullptr)
						function->argument = argDef;
					if (curArgIndex != f.Arguments.size() - 1) {
						argDef->nextArgument = new M4::HLSLArgument();
						argDef = argDef->nextArgument;
					}

					curArgIndex++;
				}

				m_immFuncs.push_back(function);
				parser.DeclareFunction(function);
			}

			// structure definitions
			for (const auto& s : m_structures)
			{
				M4::HLSLStruct* structure = new M4::HLSLStruct();
				string_pool.push_back(stralloc(s.Name));
				structure->name = string_pool[string_pool.size() - 1];

				M4::HLSLStructField* lastField = NULL;

				// Add the struct to our list of user defined types.
				for (const auto& f : s.Members)
				{
					M4::HLSLStructField* field = new M4::HLSLStructField();

					string_pool.push_back(stralloc(f.Name));
					field->name = string_pool[string_pool.size() - 1];
					field->type.baseType = M4::HLSLParser::GetTypeFromString(f.Type);
					field->type.typeName = f.Type.c_str();

					if (lastField == NULL)
						structure->field = field;
					else
						lastField->nextField = field;
					lastField = field;
				}

				m_immStructs.push_back(structure);
				parser.DeclareStructure(structure);
			}
		}

		if (!parser.Parse(&tree))
		{
			m_lastErrorMsg = hlslCompilerLastErrorMsg;
			m_immGlobals.clear();
			return false;
		}

		bool res = translate(&tree);

		for (auto& str : string_pool)
			free(str);
		m_immGlobals.clear();
		string_pool.clear();
		m_freeImmediate();

		if (!res)
			m_lastErrorMsg = "Failed to translate HLSL code to BlueVM bytecode";

		return res;
	}
	void HLSLCompiler::ClearImmediate()
	{

	}
	void HLSLCompiler::AddImmediateGlobalDefinition(Variable var)
	{
		M4::HLSLType baseType(M4::HLSLParser::GetTypeFromString(var.Type));
		m_immGlobals.push_back(std::make_pair(var.Name, std::make_pair(baseType, var.Type)));
	}

	void HLSLCompiler::m_generateConvert(HLSLCompiler::ExpressionType etype, int args)
	{
		if (etype.Type == ag::Type::Object) {
			// TODO: ?
			// EXAMPLE: (VSOutput)0; -> memset data to 0
		}
		else if (etype.Type == ag::Type::Void) {}
		else {
			if (etype.Columns * etype.Rows == 1)
				m_gen.Function.Convert(etype.Type);
			else
				m_gen.Function.NewObjectByName(etype.Name, args); // example: ivec3(vec3())
		}
	}

	bool HLSLCompiler::translate(M4::HLSLTree* tree)
	{
		M4::HLSLRoot* root = tree->GetRoot();
		translateStatements(root->statement);

		return true;
	}
	void HLSLCompiler::translateExpressionList(M4::HLSLExpression* expression)
	{
		int numExpressions = 0;
		while (expression != NULL)
		{
			translateExpression(expression);
			expression = expression->nextExpression;
			++numExpressions;
		}
	}

	void HLSLCompiler::translateExpression(M4::HLSLExpression* expression)
	{
		m_exportLine((M4::HLSLNode*)expression);

		if (expression->nodeType == M4::HLSLNodeType_IdentifierExpression)
		{
			M4::HLSLIdentifierExpression* identifierExpression = static_cast<M4::HLSLIdentifierExpression*>(expression);
			translateIdentifierExpression(identifierExpression);
		}
		else if (expression->nodeType == M4::HLSLNodeType_CastingExpression)
		{
			M4::HLSLCastingExpression* castingExpression = static_cast<M4::HLSLCastingExpression*>(expression);
			translateCastingExpression(castingExpression);
		}
		else if (expression->nodeType == M4::HLSLNodeType_ConstructorExpression)
		{
			M4::HLSLConstructorExpression* constructorExpression = static_cast<M4::HLSLConstructorExpression*>(expression);

			// TODO: move this to translateConstructor
			int argc = 0;
			M4::HLSLExpression* arg = constructorExpression->argument;
			while (arg) {
				arg = arg->nextExpression;
				argc++;
			}

			int index = argc-1;
			if (argc != 0) {
				while (index >= 0)
				{
					arg = constructorExpression->argument;
					for (int i = 0; i < index; i++)
						arg = arg->nextExpression;
					translateExpression(arg);
					index--;
				}
			}

			m_generateConvert(m_convertType(constructorExpression->type), argc);
		}
		else if (expression->nodeType == M4::HLSLNodeType_LiteralExpression)
		{
			M4::HLSLLiteralExpression* literalExpression = static_cast<M4::HLSLLiteralExpression*>(expression);
			translateLiteralExpression(literalExpression);
		}
		else if (expression->nodeType == M4::HLSLNodeType_UnaryExpression)
		{
			M4::HLSLUnaryExpression* unaryExpression = static_cast<M4::HLSLUnaryExpression*>(expression);
			translateUnaryExpression(unaryExpression);
		}
		else if (expression->nodeType == M4::HLSLNodeType_BinaryExpression)
		{
			M4::HLSLBinaryExpression* binaryExpression = static_cast<M4::HLSLBinaryExpression*>(expression);

			bool isAssign = false;
			switch (binaryExpression->binaryOp)
			{
			case M4::HLSLBinaryOp_Assign:
			case M4::HLSLBinaryOp_AddAssign:
			case M4::HLSLBinaryOp_SubAssign:
			case M4::HLSLBinaryOp_MulAssign:
			case M4::HLSLBinaryOp_DivAssign:
				isAssign = true;
				break;
			}

			if (isAssign) translateAssignExpression(binaryExpression);
			else translateBinaryExpression(binaryExpression);
		}
		else if (expression->nodeType == M4::HLSLNodeType_ConditionalExpression)
		{
			M4::HLSLConditionalExpression* conditionalExpression = static_cast<M4::HLSLConditionalExpression*>(expression);
			translateTernary(conditionalExpression);
		}
		else if (expression->nodeType == M4::HLSLNodeType_MemberAccess)
		{
			M4::HLSLMemberAccess* memberAccess = static_cast<M4::HLSLMemberAccess*>(expression);
			translateMemberAccess(memberAccess);
		}
		else if (expression->nodeType == M4::HLSLNodeType_MethodCall)
		{
			M4::HLSLMethodCall* methodCall = static_cast<M4::HLSLMethodCall*>(expression);

			translateMethodCall(methodCall);
		}
		else if (expression->nodeType == M4::HLSLNodeType_ArrayAccess)
		{
			M4::HLSLArrayAccess* arrayAccess = static_cast<M4::HLSLArrayAccess*>(expression);
			translateArrayAccess(arrayAccess);
		}
		else if (expression->nodeType == M4::HLSLNodeType_FunctionCall)
		{
			M4::HLSLFunctionCall* functionCall = static_cast<M4::HLSLFunctionCall*>(expression);
			translateFunctionCall(functionCall);
		}
		else { }
	}
	
	void HLSLCompiler::translateOperator(M4::HLSLBinaryOp op)
	{
		switch (op)
		{
		case M4::HLSLBinaryOp_Add:          m_gen.Function.Add(); break;
		case M4::HLSLBinaryOp_Sub:          m_gen.Function.Subtract(); break;
		case M4::HLSLBinaryOp_Mul:          m_gen.Function.Multiply(); break;
		case M4::HLSLBinaryOp_Mod:          m_gen.Function.Modulo(); break;
		case M4::HLSLBinaryOp_Div:          m_gen.Function.Divide(); break;
		case M4::HLSLBinaryOp_Less:         m_gen.Function.Less(); break;
		case M4::HLSLBinaryOp_Greater:      m_gen.Function.Greater(); break;
		case M4::HLSLBinaryOp_BitShiftLeft: m_gen.Function.BitLeftShift(); break;
		case M4::HLSLBinaryOp_BitShiftRight:m_gen.Function.BitRightShift(); break;
		case M4::HLSLBinaryOp_LessEqual:    m_gen.Function.LessEqual(); break;
		case M4::HLSLBinaryOp_GreaterEqual: m_gen.Function.GreaterEqual(); break;
		case M4::HLSLBinaryOp_Equal:        m_gen.Function.Equal(); break;
		case M4::HLSLBinaryOp_NotEqual:     m_gen.Function.NotEqual(); break;
		case M4::HLSLBinaryOp_And:          m_gen.Function.BoolAnd(); break;
		case M4::HLSLBinaryOp_Or:           m_gen.Function.BoolOr(); break;
		case M4::HLSLBinaryOp_BitAnd:       m_gen.Function.BitAnd(); break;
		case M4::HLSLBinaryOp_BitOr:        m_gen.Function.BitOr(); break;
		case M4::HLSLBinaryOp_BitXor:       m_gen.Function.BitXor(); break;
		}
	}
	void HLSLCompiler::translateBinaryExpression(M4::HLSLBinaryExpression* expr)
	{
		HLSLCompiler::ExpressionType lType = m_convertType(expr->expression1->expressionType);
		HLSLCompiler::ExpressionType rType = m_convertType(expr->expression2->expressionType);

		// call fmod() when using modulo with floats
		if (expr->binaryOp == M4::HLSLBinaryOp_Mod &&
			(rType.Type == ag::Type::Float || lType.Type == ag::Type::Float) &&
			(rType.Columns * rType.Rows == 1 && lType.Columns * lType.Rows == 1))
		{
			translateExpression(expr->expression2);
			if (lType.Columns * lType.Rows > 1 && rType.Columns * rType.Rows == 1)
				m_generateConvert(lType);

			translateExpression(expr->expression1);
			if (rType.Columns * rType.Rows > 1 && lType.Columns * lType.Rows == 1)
				m_generateConvert(rType);

			m_gen.Function.CallReturn("fmod", 2);
		}
		// else generate operator opcode
		else {
			translateExpression(expr->expression1);
			if (rType.Columns * rType.Rows > 1 && lType.Columns * lType.Rows == 1)
				m_generateConvert(rType);

			translateExpression(expr->expression2);
			if (lType.Columns * lType.Rows > 1 && rType.Columns * rType.Rows == 1)
				m_generateConvert(lType);

			translateOperator(expr->binaryOp);
		}
	}
	void HLSLCompiler::translateAssignExpression(M4::HLSLBinaryExpression* expr)
	{
		HLSLCompiler::ExpressionType lType = m_convertType(expr->expression1->expressionType);
		HLSLCompiler::ExpressionType rType = m_convertType(expr->expression2->expressionType);

		if (expr->binaryOp != M4::HLSLBinaryOp_Assign) { // push the lhs to the stack too if we are using some +=, -=, etc... operator
			translateExpression(expr->expression1);
			if (rType.Columns * rType.Rows > 1 && lType.Columns * lType.Rows == 1)
				m_generateConvert(rType);
		}
		translateExpression(expr->expression2); // rhs
		if (lType.Columns * lType.Rows > 1 && rType.Columns * rType.Rows == 1)
			m_generateConvert(lType);

		if (expr->binaryOp == M4::HLSLBinaryOp_AddAssign)
			m_gen.Function.Add();
		else if (expr->binaryOp == M4::HLSLBinaryOp_SubAssign)
			m_gen.Function.Subtract();
		else if (expr->binaryOp == M4::HLSLBinaryOp_MulAssign)
			m_gen.Function.Multiply();
		else if (expr->binaryOp == M4::HLSLBinaryOp_DivAssign)
			m_gen.Function.Divide();

		if (lType != rType)
			m_generateConvert(lType);

		m_isSet = true;
		translateExpression(expr->expression1); // lhs
		m_isSet = false;
	}
	void HLSLCompiler::translateUnaryExpression(M4::HLSLUnaryExpression* expr)
	{
		// check if pre or post increment/decrement
		bool pre = true, isIncDec = false;
		switch (expr->unaryOp)
		{
		case M4::HLSLUnaryOp_PostIncrement: isIncDec = true; pre = false; break;
		case M4::HLSLUnaryOp_PostDecrement: isIncDec = true; pre = false; break;
		case M4::HLSLUnaryOp_PreIncrement: isIncDec = true; break;
		case M4::HLSLUnaryOp_PreDecrement: isIncDec = true; break;
		}

		if (isIncDec && pre)
			m_usePointer = true;
		translateExpression(expr->expression);
		m_usePointer = false;

		// add the pointer
		if (isIncDec && !pre) {
			m_usePointer = true;
			translateExpression(expr->expression);
			m_usePointer = false;
		}

		switch (expr->unaryOp)
		{
		case M4::HLSLUnaryOp_Negative: m_gen.Function.Negate(); break;
		case M4::HLSLUnaryOp_Positive: break; // hmm.. do nothing?
		case M4::HLSLUnaryOp_Not: m_gen.Function.Not(); break;
		case M4::HLSLUnaryOp_PostIncrement:
		case M4::HLSLUnaryOp_PreIncrement: m_gen.Function.Increment(); break;
		case M4::HLSLUnaryOp_PostDecrement:
		case M4::HLSLUnaryOp_PreDecrement: m_gen.Function.Decrement(); break;
		case M4::HLSLUnaryOp_BitNot: m_gen.Function.BitNot(); break;
		}

		// add the incremented/dec value to the stack again
		if (isIncDec && pre)
			translateExpression(expr->expression);

		// remove the pointer in the post increment/dec
		if (isIncDec && !pre)
			m_gen.Function.PopStack();
	}
	void HLSLCompiler::translateIdentifierExpression(M4::HLSLIdentifierExpression* expression)
	{
		const std::string vname = std::string(expression->name);
		bool found = false;

		if (m_currentFunction.size() != 0 && !found) {
			// locals
			for (int i = 0; i < m_locals[m_currentFunction].size(); i++) {
				if (m_locals[m_currentFunction][i] == vname) {
					if (!m_isSet) {
						if (m_usePointer) m_gen.Function.GetLocalPointer(i);
						else m_gen.Function.GetLocal(i);
					}
					else
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
					if (m_func[i].Arguments[j].Name == vname) {
						bool isPtr = m_func[i].Arguments[j].Storage == sd::Variable::StorageType::Out;

						if (!m_isSet) {
							if (m_usePointer) m_gen.Function.GetArgumentPointer(j);
							else m_gen.Function.GetArgument(j);
						}
						else {
							if (isPtr) {
								m_gen.Function.GetArgument(j);
								m_gen.Function.Assign();
							} else
								m_gen.Function.SetArgument(j);
						}

						found = true;
						break;
					}
				}
			}
		}

		if (!found) {
			// globals
			for (int i = 0; i < m_globals.size(); i++)
				if (m_globals[i].Name == vname) {
					if (!m_isSet) {
						if (m_usePointer) m_gen.Function.GetGlobalPointer(m_globals[i].ID);
						else m_gen.Function.GetGlobal(m_globals[i].ID);
					}
					else
						m_gen.Function.SetGlobal(m_globals[i].ID);

					found = true;
					break;
				}
		}

		if (!found) {
			if (!m_isSet) {
				if (m_usePointer) m_gen.Function.GetGlobalPointerByName(vname);
				else m_gen.Function.GetGlobalByName(vname);
			}
			else
				m_gen.Function.SetGlobalByName(vname);
		}
	}
	void HLSLCompiler::translateCastingExpression(M4::HLSLCastingExpression* expression)
	{
		// (VSInput)0
		if (expression->type.baseType == M4::HLSLBaseType_UserDefined) {
			M4::HLSLExpression* val = expression->expression;
			std::string sname = expression->type.typeName;

			// just use default constructor for now
			m_gen.Function.NewObjectByName(sname);
		}
		// (float)2.5f
		else {
			translateExpression(expression->expression);
			m_convertType(expression->type);
		}
	}
	void HLSLCompiler::translateArguments(M4::HLSLArgument* argument)
	{
		m_curFuncData->Arguments.clear();

		M4::HLSLArgument* arg = argument;

		int index = 0;
		while (arg != NULL) {
			Variable pdata;
			pdata.ID = index;
			pdata.Name = arg->name;
			pdata.Type = m_convertType(arg->type).Name;
			pdata.Semantic = arg->sv_semantic ? arg->sv_semantic : (arg->semantic ? arg->semantic : "");
			
			switch (arg->modifier)
			{
			case M4::HLSLArgumentModifier_In: pdata.Storage = Variable::StorageType::In; break;
			case M4::HLSLArgumentModifier_Out: pdata.Storage = Variable::StorageType::Out; break;
			case M4::HLSLArgumentModifier_Inout: pdata.Storage = Variable::StorageType::InOut; break;
			case M4::HLSLArgumentModifier_Uniform: pdata.Storage = Variable::StorageType::Uniform; break;
			case M4::HLSLArgumentModifier_Const: pdata.Storage = Variable::StorageType::Constant; break;
			default: pdata.Storage = Variable::StorageType::None; break;
			}
			
			m_curFuncData->Arguments.push_back(pdata);

			arg = arg->nextArgument;
		}
	}
	void HLSLCompiler::translateLiteralExpression(M4::HLSLLiteralExpression* expr)
	{
		switch (expr->type)
		{
		case M4::HLSLBaseType_Half:
		case M4::HLSLBaseType_Float:
			m_gen.Function.PushStack(expr->fValue);
		break;
		case M4::HLSLBaseType_Int:
			m_gen.Function.PushStack(expr->iValue);
			break;
		case M4::HLSLBaseType_Bool:
			m_gen.Function.PushStack(expr->bValue);
			break;
		default:
			ASSERT(0);
		}
	}
	void HLSLCompiler::translateTernary(M4::HLSLConditionalExpression* expr)
	{
		translateExpression(expr->condition);
		size_t pos = m_gen.Function.If();
		// ?
		translateExpression(expr->trueExpression);
		size_t goto_skip = m_gen.Function.Goto();
		// :
		m_gen.Function.SetAddress(pos, m_gen.Function.GetCurrentAddress());
		translateExpression(expr->falseExpression);

		m_gen.Function.SetAddress(goto_skip, m_gen.Function.GetCurrentAddress());
	}
	void HLSLCompiler::translateMemberAccess(M4::HLSLMemberAccess* expr)
	{
		bool temp_isSet = m_isSet, temp_usePointer = m_usePointer;
		m_isSet = false;
		m_usePointer = true;
		translateExpression(expr->object);
		m_isSet = temp_isSet;
		m_usePointer = temp_usePointer;

		if (m_isSet) m_gen.Function.SetProperty(expr->field);
		else {
			if (m_usePointer) m_gen.Function.GetPropertyPointer(expr->field);
			else m_gen.Function.GetProperty(expr->field);
		}
	}
	void HLSLCompiler::translateMethodCall(M4::HLSLMethodCall* expr)
	{
		// arguments
		bool temp_isSet = m_isSet, temp_usePointer = m_usePointer;
		m_isSet = false;
		m_usePointer = true;
		for (int i = expr->numArguments - 1; i >= 0; i--) {
			M4::HLSLExpression* arg = expr->argument;
			for (int j = 0; j < i; j++)
				arg = arg->nextExpression;
			translateExpression(arg);
		}
		m_isSet = temp_isSet;
		m_usePointer = temp_usePointer;

		// object
		m_isSet = false;
		m_usePointer = true;
		translateExpression(expr->object);
		m_isSet = temp_isSet;
		m_usePointer = temp_usePointer;

		// call method
		m_gen.Function.CallReturnMethod(expr->function->name, expr->numArguments);
	}
	void HLSLCompiler::translateAttributes(M4::HLSLAttribute* attribute)
	{
		while (attribute != NULL)
		{
			attribute = attribute->nextAttribute;
		}
	}
	void HLSLCompiler::translateArrayAccess(M4::HLSLArrayAccess* expr)
	{
		translateExpression(expr->index);

		bool temp_isSet = m_isSet;
		m_isSet = false;
		translateExpression(expr->array);
		m_isSet = temp_isSet;

		if (m_isSet) {
			m_gen.Function.SetArrayElement();
			translateExpression(expr->array);
		} else
			m_gen.Function.GetArrayElement();
	}
	void HLSLCompiler::translateFunctionCall(M4::HLSLFunctionCall* expr)
	{
		// mul(a,b)
		if (strcmp(expr->function->name, "mul") == 0 && expr->numArguments == 2) {
			// handle mul() with * operator

			HLSLCompiler::ExpressionType lType = m_convertType(expr->argument->expressionType);
			HLSLCompiler::ExpressionType rType = m_convertType(expr->argument->nextExpression->expressionType);

			translateExpression(expr->argument);
			if (rType.Columns * rType.Rows > 1 && lType.Columns * lType.Rows == 1)
				m_generateConvert(rType);

			translateExpression(expr->argument->nextExpression);
			if (lType.Columns * lType.Rows > 1 && rType.Columns * rType.Rows == 1)
				m_generateConvert(lType);

			translateOperator(M4::HLSLBinaryOp_Mul);
		}
		// other functions
		else {
			Function data = m_matchFunction(expr->function->name, expr->numArguments, expr->argument);
			std::vector<HLSLCompiler::ExpressionType> paramTypes(expr->numArguments);
			for (int i = 0; i < data.Arguments.size(); i++)
				paramTypes[i] = m_convertType(data.Arguments[i].Type);

			for (int i = expr->numArguments - 1; i >= 0; i--) {
				bool temp_usePointer = m_usePointer;

				// built-in funcs with 'out' keyword
				if (data.Name.empty()) {
					if (m_builtInFuncsPtrs.count(expr->function->name) > 0)
						if (std::count(m_builtInFuncsPtrs[expr->function->name].begin(), m_builtInFuncsPtrs[expr->function->name].end(), i) > 0)
							m_usePointer = true;
				}
				// user defined funcs with 'out' keyword
				else if (data.Arguments[i].Storage == sd::Variable::StorageType::Out)
					m_usePointer = true;


				M4::HLSLExpression* arg = expr->argument;
				for (int j = 0; j < i; j++)
					arg = arg->nextExpression;
				translateExpression(arg);

				m_usePointer = temp_usePointer;

				if (!data.Name.empty() && paramTypes[i] != m_convertType(arg->expressionType))
					m_generateConvert(paramTypes[i]);
			}


			m_gen.Function.CallReturn(expr->function->name, expr->numArguments);
		}
	}

	void HLSLCompiler::translateStatements(M4::HLSLStatement* statement)
	{
		while (statement != NULL)
		{
			if (statement->hidden) 
			{
				statement = statement->nextStatement;
				continue;
			}

			m_exportLine((M4::HLSLNode*)statement);

			// TODO: flatten, unroll?
			// translateAttributes(statement->attributes);

			if (statement->nodeType == M4::HLSLNodeType_Declaration)
			{
				M4::HLSLDeclaration* declaration = static_cast<M4::HLSLDeclaration*>(statement);
				translateDeclaration(declaration);

				declaration = declaration->nextDeclaration;

				while (declaration != NULL) {
					translateDeclaration(declaration); 
					declaration = declaration->nextDeclaration;
				}
			}
			else if (statement->nodeType == M4::HLSLNodeType_Struct)
			{
				M4::HLSLStruct* structure = static_cast<M4::HLSLStruct*>(statement);
				translateStructure(structure);
			}
			else if (statement->nodeType == M4::HLSLNodeType_Buffer)
			{
				M4::HLSLBuffer* buffer = static_cast<M4::HLSLBuffer*>(statement);
				M4::HLSLDeclaration* field = buffer->field;

				m_isInsideBuffer = true;

				while (field != NULL)
				{
					// just add cbuffer members as a global variables
					if (!field->hidden)
						translateDeclaration(field);
					field = (M4::HLSLDeclaration*)field->nextDeclaration;
				}

				m_isInsideBuffer = false;
			}
			else if (statement->nodeType == M4::HLSLNodeType_Function)
			{
				M4::HLSLFunction* function = static_cast<M4::HLSLFunction*>(statement);
				translateFunction(function);
			}
			else if (statement->nodeType == M4::HLSLNodeType_ExpressionStatement)
			{
				M4::HLSLExpressionStatement* expressionStatement = static_cast<M4::HLSLExpressionStatement*>(statement);
				translateExpression(expressionStatement->expression);
			}
			else if (statement->nodeType == M4::HLSLNodeType_ReturnStatement)
			{
				M4::HLSLReturnStatement* returnStatement = static_cast<M4::HLSLReturnStatement*>(statement);
				translateReturnStatement(returnStatement);
			}
			else if (statement->nodeType == M4::HLSLNodeType_DiscardStatement)
			{
				m_gen.Function.Call("$$discard", 0);
				m_gen.Function.Return();
			}
			else if (statement->nodeType == M4::HLSLNodeType_BreakStatement)
			{
				M4::HLSLBreakStatement* breakStatement = static_cast<M4::HLSLBreakStatement*>(statement);
				translateBreakStatement(breakStatement);
			}
			else if (statement->nodeType == M4::HLSLNodeType_ContinueStatement)
			{
				M4::HLSLContinueStatement* continueStatement = static_cast<M4::HLSLContinueStatement*>(statement);
				translateContinueStatement(continueStatement);
			}
			else if (statement->nodeType == M4::HLSLNodeType_IfStatement)
			{
				M4::HLSLIfStatement* ifStatement = static_cast<M4::HLSLIfStatement*>(statement);
				translateIfStatement(ifStatement);
			}
			else if (statement->nodeType == M4::HLSLNodeType_ForStatement)
			{
				M4::HLSLForStatement* forStatement = static_cast<M4::HLSLForStatement*>(statement);
				translateForStatement(forStatement);
			}
			else if (statement->nodeType == M4::HLSLNodeType_BlockStatement)
			{
				M4::HLSLBlockStatement* blockStatement = static_cast<M4::HLSLBlockStatement*>(statement);

				// m_gen.Function.ScopeStart()

				translateStatements(blockStatement->statement);

				// m_gen.Function.ScopeEnd()
			}
			else
			{
				// Unhanded statement type.
				ASSERT(0);
			}

			statement = statement->nextStatement;
		}
	}

	void HLSLCompiler::translateStructure(M4::HLSLStruct* structure)
	{
		m_structures.push_back(Structure());
		Structure& str = m_structures[m_structures.size() - 1];
		str.Name = structure->name;
		str.ID = m_gen.AddObject(str.Name);

		M4::HLSLStructField* field = structure->field;
		while (field != NULL)
		{
			if (!field->hidden)
			{
				Variable fieldData;

				fieldData.Name = field->name;
				m_gen.AddProperty(str.Name, fieldData.Name);

				M4::HLSLType ftype = field->type;
				HLSLCompiler::ExpressionType etype = m_convertType(ftype);
				fieldData.Type = etype.Name;
				fieldData.Semantic = field->sv_semantic ? field->sv_semantic : (field->semantic ? field->semantic : "");
				fieldData.IsArray = ftype.array;
				fieldData.Flat = ftype.flags & M4::HLSLTypeFlag_NoInterpolation;
				
				str.Members.push_back(fieldData);
			}
			field = field->nextField;
		}
	}
	void HLSLCompiler::translateFunction(M4::HLSLFunction* func)
	{
		m_func.push_back(Function());
		m_curFuncData = &m_func[m_func.size() - 1];
		m_curFuncData->Arguments.clear();

		// add arguments
		translateArguments(func->argument);

		// build list for internal workings
		M4::HLSLArgument* arg = func->argument;
		int index = 0;
		std::vector<ag::Type> argTypes;
		std::vector<std::string> argNames;
		while (arg != NULL) {
			HLSLCompiler::ExpressionType btype = m_convertType(arg->type);
			argTypes.push_back(btype.Type);
			argNames.push_back(btype.Name);
			arg = arg->nextArgument;
		}

		m_curFuncData->ID = m_gen.Function.Create(func->name, func->numArguments, argTypes, argNames);
		m_curFuncData->Name = m_currentFunction = func->name;

		m_curFuncData->ReturnType = m_convertType(func->returnType).Name;

		m_gen.Function.SetCurrent(m_curFuncData->ID);

		// initialize global variables at the start of main()
		if (m_curFuncData->Name == m_entryFunction) {
			// create global objects (vec3, etc..)
			for (auto& gInitClass : m_initObjsInMain) {

				// skip textures and sampler states (TODO: should actually check if register is set, i guess..)
				if (sd::IsBasicTexture(gInitClass.second.c_str()) || gInitClass.second == "SamplerState" || gInitClass.second == "sampler")
					continue;

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
				M4::HLSLExpression* sizeExpr = gArray.second;
				int arrDepth = 0;
				while (sizeExpr) {
					translateExpression(gArray.second);
					arrDepth++;
					sizeExpr = sizeExpr->nextExpression;
				}

				m_gen.Function.NewArray(arrDepth);
				size_t varID = 0;
				for (auto& gVar : m_globals)
					if (gVar.Name == gArray.first)
						varID = gVar.ID;
				m_gen.Function.SetGlobal(varID);
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

		m_exportLine(func);

		// function body
		translateStatements(func->statement);

		m_curFuncData = nullptr;
		m_currentFunction = "";
	}
	void HLSLCompiler::translateReturnStatement(M4::HLSLReturnStatement* ret)
	{
		if (ret->expression != NULL) {
			translateExpression(ret->expression);

			if (!m_isImmediate) {
				HLSLCompiler::ExpressionType rType = m_convertType(m_curFuncData->ReturnType);
				HLSLCompiler::ExpressionType expType = m_convertType(ret->expression->expressionType);

				if (rType != expType)
					m_generateConvert(rType);
			}
		}

		m_gen.Function.Return();
	}
	void HLSLCompiler::translateBreakStatement(M4::HLSLBreakStatement* brk)
	{
		if (m_breaks.size() > 0)
			m_breaks.top().push_back(m_gen.Function.Goto());
	}
	void HLSLCompiler::translateContinueStatement(M4::HLSLContinueStatement* brk)
	{
		m_gen.Function.SetAddress(m_gen.Function.Goto(), m_continueAddr.top());
	}
	void HLSLCompiler::translateIfStatement(M4::HLSLIfStatement* ifStmt)
	{
		translateExpression(ifStmt->condition);
		size_t pos = m_gen.Function.If();
		translateStatements(ifStmt->statement);
		size_t goto_ref = m_gen.Function.Goto();
		m_gen.Function.SetAddress(pos, m_gen.Function.GetCurrentAddress());

		if (ifStmt->elseStatement)
			translateStatements(ifStmt->elseStatement);

		m_gen.Function.SetAddress(goto_ref, m_gen.Function.GetCurrentAddress());
	}
	void HLSLCompiler::translateForStatement(M4::HLSLForStatement* forStmt)
	{
		m_breaks.push(std::vector<size_t>());

		if (forStmt->initialization)
			translateDeclaration(forStmt->initialization);

		int first_test = -1;
		int reg_test = -1;
		int rewind_adr = -1;

		if (forStmt->condition) {
			translateExpression(forStmt->condition);
			first_test = m_gen.Function.If();
		}

		// on first iteration skip update() and regular condition()
		size_t body_start = m_gen.Function.Goto();
		m_continueAddr.push(m_gen.Function.GetCurrentAddress());

		// the third part of the for loop
		if (forStmt->increment)
			translateExpression(forStmt->increment);

		if (forStmt->condition) {
			translateExpression(forStmt->condition);
			reg_test = m_gen.Function.If();
		}

		m_gen.Function.SetAddress(body_start, m_gen.Function.GetCurrentAddress());
		translateStatements(forStmt->statement);

		// go to top (m_continueAddr == top) where top is where loop -> condition -> body starts
		size_t rewind_ref = m_gen.Function.Goto();
		m_gen.Function.SetAddress(rewind_ref, m_continueAddr.top());

		// address to which vm should go if condition result is false
		if (reg_test != -1)
			m_gen.Function.SetAddress(reg_test, m_gen.Function.GetCurrentAddress());
		if (first_test != -1)
			m_gen.Function.SetAddress(first_test, m_gen.Function.GetCurrentAddress());

		for (size_t i = 0; i < m_breaks.top().size(); i++)
			m_gen.Function.SetAddress(m_breaks.top()[i], m_gen.Function.GetCurrentAddress());

		m_continueAddr.pop();
		m_breaks.pop();
	}

	void HLSLCompiler::translateDeclaration(M4::HLSLDeclaration* declr)
	{
		bool isReadTextureType = M4::IsReadTextureType(declr->type);

		// local
		if (m_curFuncData != nullptr) {
			HLSLCompiler::ExpressionType lType = m_convertType(declr->type);

			// check if it already exists, if it doesn't add it
			bool alreadyExists = false;
			size_t varIndex = 0;
			for (size_t i = 0; i < m_locals[m_currentFunction].size(); i++)
				if (m_locals[m_currentFunction][i] == declr->name) {
					varIndex = i;
					alreadyExists = true;
					break;
				}
			if (!alreadyExists) {
				m_locals[m_currentFunction].push_back(declr->name);
				m_localTypes[m_currentFunction][std::string(declr->name)] = lType.Name;
				varIndex = m_locals[m_currentFunction].size() - 1;
			}
			
			if (declr->type.array) {
				translateExpression(declr->type.arraySize);
				u8 dim = 0;
				M4::HLSLExpression* expr = declr->type.arraySize;
				while (expr != nullptr) {
					expr = expr->nextExpression;
					dim++;
				}
				m_gen.Function.NewArray(dim);
				m_gen.Function.SetLocal(varIndex);
			}
			else if (declr->assignment) {
				translateExpression(declr->assignment);

				HLSLCompiler::ExpressionType rType = m_convertType(declr->assignment->expressionType);
				if (lType != rType)
					m_generateConvert(lType);

				m_gen.Function.SetLocal(varIndex);
			}
			else if (m_isTypeActuallyStruct(lType)) {
				m_gen.Function.NewObjectByName(lType.Name);
				m_gen.Function.SetLocal(varIndex);
			}
			else {
				m_gen.Function.PushStack(0);
				m_generateConvert(lType);
				m_gen.Function.SetLocal(varIndex);
			}
		}
		// global
		else {
			const M4::HLSLType type = declr->type;

			m_globals.push_back(Variable());

			Variable& var = m_globals[m_globals.size() - 1];
			var.InputSlot = declr->registerName ? std::stoi(declr->registerName + 1) : 0;
			var.IsArray = type.array;
			var.Smooth = true;
			var.Flat = false;
			var.NoPerspective = false;
			var.Storage = Variable::StorageType::Constant;
			var.Name = declr->name;

			// Interpolation modifiers.
			if (type.flags & M4::HLSLTypeFlag_Centroid) { }
			if (type.flags & M4::HLSLTypeFlag_Linear) { }
			if (type.flags & M4::HLSLTypeFlag_NoInterpolation) { }
			if (type.flags & M4::HLSLTypeFlag_NoPerspective) var.NoPerspective = false;
			if (type.flags & M4::HLSLTypeFlag_Sample) { }
			
			HLSLCompiler::ExpressionType expType = m_convertType(declr->type);
			var.Type = expType.Name;

			if (m_isInsideBuffer || sd::IsBasicTexture(var.Type.c_str())) {
				var.Storage = Variable::StorageType::Uniform;
				// if (type.flags & M4::HLSLTypeFlag_Const) var.Storage = Variable::StorageType::Constant;
				// if (type.flags & M4::HLSLTypeFlag_Static) {}
			}

			if (m_isTypeActuallyStruct(expType) && var.Storage != Variable::StorageType::Uniform)
				m_initObjsInMain[var.Name] = var.Type;

			if (var.IsArray)
				m_initArraysInMain[var.Name] = type.arraySize;

			// generate bytecode
			var.ID = m_gen.AddGlobal(var.Name);

			if (declr->assignment)
				m_initInMain[var.Name] = declr->assignment;

		}
	}

}
