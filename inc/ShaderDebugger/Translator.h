#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include <aGen/aGen.hpp>

#include <ShaderDebugger/ShaderType.h>
#include <ShaderDebugger/Function.h>
#include <ShaderDebugger/Structure.h>
#include <ShaderDebugger/Variable.h>

extern "C" {
	#include <BlueVM/bv_object.h>
}

namespace sd
{
	class Translator
	{
	public:
		inline ag::Generator& GetBytecodeGenerator() { return m_gen; }
		inline std::vector<uint8_t> GetBytecode() { return m_gen.Get().Get(); }
		inline const std::vector<Variable>& GetGlobals() { return m_globals; }
		inline const std::vector<Structure>& GetStructures() { return m_structures; }
		inline const std::vector<Function>& GetFunctions() { return m_func; }
		inline const std::vector<std::string>& GetLocals(const std::string& func) { return m_locals[func]; }
		inline const std::string& GetLocalType(const std::string& func, const std::string& var) { return m_localTypes[func][var]; }

		inline void SetImmediate(bool s) { m_isImmediate = s; }
		inline bool GetImmediate() { return m_isImmediate; }
		inline void AddFunctionDefinition(const Function& func) { m_func.push_back(func); }
		inline void AddStructureDefinition(const Structure& str) { m_structures.push_back(str); }
		inline void AddGlobalDefinition(const Variable& var) { m_globals.push_back(var); }
		inline void ClearDefinitions() { m_globals.clear(); m_structures.clear(); m_func.clear(); m_locals.clear(); m_localTypes.clear(); ClearImmediate(); }

		virtual bool Parse(ShaderType type, const std::string& source, std::string entry = "main") = 0;

		virtual void ClearImmediate() = 0;
		virtual void AddImmediateGlobalDefinition(Variable var) = 0;

		bv_object_get_property_ext PropertyGetter;

	protected:
		bool m_isImmediate;
		ag::Generator m_gen;
		std::vector<Variable> m_globals;
		std::vector<Structure> m_structures;
		std::vector<Function> m_func;
		std::unordered_map<std::string, std::vector<std::string>> m_locals;
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_localTypes;
	};
}