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

		virtual bool Parse(ShaderType type, const std::string& source, std::string entry = "main") = 0;

		bv_object_get_property_ext PropertyGetter;

	protected:
		ag::Generator m_gen;
		std::vector<Variable> m_globals;
		std::vector<Structure> m_structures;
		std::vector<Function> m_func;
		std::unordered_map<std::string, std::vector<std::string>> m_locals;
	};
}