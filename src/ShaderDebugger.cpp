#include <ShaderDebugger/ShaderDebugger.h>

namespace sd
{
	ShaderDebugger::ShaderDebugger()
	{
		m_prog = nullptr;
		m_library = nullptr;
		m_stepper = nullptr;
	}
	ShaderDebugger::~ShaderDebugger()
	{
		//if (m_stepper != nullptr)
		//	bv_function_stepper_delete(m_stepper);
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