#include <ShaderDebugger/ShaderDebugger.h>

namespace sd
{
	ShaderDebugger::ShaderDebugger()
	{
		m_prog = nullptr;
		m_library = nullptr;
	}
	ShaderDebugger::~ShaderDebugger()
	{
		if (m_prog != nullptr)
			bv_program_delete(m_prog);
	}
	bv_variable ShaderDebugger::Execute(const std::string& func)
	{
		bv_function* funcPtr = bv_program_get_function(m_prog, func.c_str());
		if (funcPtr == nullptr)
			return bv_variable_create_void(); // function doesn't exist


		// call function and store its return value
		return bv_program_call(m_prog, funcPtr, NULL, NULL);
	}
	bv_variable* ShaderDebugger::GetValue(const std::string& gvarname)
	{
		return bv_program_get_global(m_prog, const_cast<char*>(gvarname.c_str())); // TODO: why does get_global need non const??
	}
}