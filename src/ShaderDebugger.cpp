#include <ShaderDebugger/ShaderDebugger.h>

namespace sd
{
	ShaderDebugger::ShaderDebugger()
	{
		m_prog = nullptr;
		m_library = nullptr;
		m_stepper = nullptr;
		m_transl = nullptr;
	}
	ShaderDebugger::~ShaderDebugger()
	{
		if (m_stepper != nullptr)
			bv_function_stepper_delete(m_stepper);
		if (m_prog != nullptr)
			bv_program_delete(m_prog);
		if (m_transl != nullptr)
			delete m_transl;
		if (m_library != nullptr)
			bv_library_delete(m_library);
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
	void ShaderDebugger::SetValue(const std::string& varName, float value)
	{
		bv_program_set_global(m_prog, varName.c_str(), bv_variable_create_float(value));
	}
	void ShaderDebugger::SetValue(const std::string& varName, const std::string& classType, glm::vec3 val)
	{
		bv_object_info* objInfo = bv_program_get_object_info(m_prog, classType.c_str());
		bv_variable objVar = bv_variable_create_object(objInfo);
		bv_object* obj = bv_variable_get_object(objVar);

		bv_object_set_property(obj, "x", bv_variable_create_float(val.x));
		bv_object_set_property(obj, "y", bv_variable_create_float(val.y));
		bv_object_set_property(obj, "z", bv_variable_create_float(val.z));

		bv_program_set_global(m_prog, varName.c_str(), objVar);
	}

	std::string ShaderDebugger::GetCurrentFunction()
	{
		if (m_stepper->scope->count == 0)
			return "";

		bv_state* state = bv_scope_get_state(m_stepper->scope);
		bv_program* prog = state->prog;
		u16 func_count = bv_program_get_function_count(prog);

		for (u16 i = 0; i < func_count; i++)
			if (prog->functions[i] == state->func)
				return prog->block->functions->names[i];

		return "";
	}
	std::vector<std::string> ShaderDebugger::GetFunctionStack()
	{
		if (m_stepper->scope->count == 0)
			return std::vector<std::string>();
			
		std::vector<std::string> ret;
		bv_scope* scope = m_stepper->scope;

		// I think there was an easier way to get the function stack
		// but I am not sure (TODO - check)
		for (u32 j = 0; j < scope->count; j++) {
			bv_state* state = &scope->state[j];
			bv_program* prog = state->prog;
			u16 func_count = bv_program_get_function_count(prog);
			for (u16 i = 0; i < func_count; i++)
				if (prog->functions[i] == state->func)
					ret.push_back(prog->block->functions->names[i]);
		}

		return ret;
	}
	std::vector<std::string> ShaderDebugger::GetCurrentFunctionLocals()
	{
		std::string curFunc = GetCurrentFunction();
		if (curFunc.size() == 0)
			return std::vector<std::string>();
		return m_transl->GetLocals(curFunc);
	}
	bv_variable* ShaderDebugger::GetLocalValue(const std::string& varname)
	{
		bv_scope* scope = m_stepper->scope;
		std::string curFunc = GetCurrentFunction();
		const auto& locList = m_transl->GetLocals(curFunc);

		for (u32 i = 0; i < locList.size(); i++)
			if (locList[i] == varname) {
				u32 index = bv_scope_get_locals_start(scope)+i;

				if (index >= scope->locals.length)
					return nullptr;
				
				return &scope->locals.data[index];
			}
		return nullptr;
	}
	bool ShaderDebugger::Step()
	{
		bool done = bv_function_stepper_is_done(m_stepper);
		if (!done) {
			int curLine = m_prog->current_line;
			bool tempDone = done;

			while (curLine == m_prog->current_line && !tempDone) {
				bv_function_step(m_stepper);
				tempDone = bv_function_stepper_is_done(m_stepper);
			}
		}

		return !done;
	}
}