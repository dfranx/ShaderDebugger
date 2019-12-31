#include <ShaderDebugger/ShaderDebugger.h>

namespace sd
{
	bv_variable DiscardFunction(bv_program* prog, u8 count, bv_variable* args)
	{
		ShaderDebugger* dbgr = (ShaderDebugger*)prog->user_data;
		dbgr->SetDiscarded(true);
		return bv_variable_create_void();
	}

	ShaderDebugger::ShaderDebugger()
	{
		m_prog = nullptr;
		m_library = nullptr;
		m_stepper = nullptr;
		m_transl = nullptr;
		m_discarded = false;
	}
	ShaderDebugger::~ShaderDebugger()
	{
		if (m_stepper != nullptr)
			bv_function_stepper_delete(m_stepper);
		if (m_prog != nullptr)
			bv_program_delete(m_prog);
		if (m_transl != nullptr)
			delete m_transl;
		if (m_library != nullptr) {
			bv_library_delete(m_library);
			m_library = nullptr;
		}
	}
	bv_variable ShaderDebugger::Execute(const std::string& func, bv_stack* args)
	{
		bv_function* funcPtr = bv_program_get_function(m_prog, func.c_str());
		if (funcPtr == nullptr)
			return bv_variable_create_void(); // function doesn't exist

		SetDiscarded(false);

		// call function and store its return value
		return bv_program_call(m_prog, funcPtr, args, NULL);
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
	void ShaderDebugger::SetValue(const std::string& varName, const std::string& classType, sd::Texture* val)
	{
		bv_object_info* objInfo = bv_program_get_object_info(m_prog, classType.c_str());
		bv_variable objVar = bv_variable_create_object(objInfo);
		bv_object* obj = bv_variable_get_object(objVar);

		obj->user_data = (void*)val;

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
		auto argList = m_getFunctionInfo(GetCurrentFunction()).Arguments;

		for (u32 i = 0; i < argList.size(); i++)
			if (argList[i].Name == varname) {
				u32 index = bv_scope_get_locals_start(scope) + i;

				if (index >= scope->locals.length)
					return nullptr;

				return &scope->locals.data[index];
			}

		for (u32 i = 0; i < locList.size(); i++)
			if (locList[i] == varname) {
				u32 index = bv_scope_get_locals_start(scope)+i+argList.size();

				if (index >= scope->locals.length)
					return nullptr;
				
				return &scope->locals.data[index];
			}
		return nullptr;
	}
	void ShaderDebugger::Jump(int line)
	{
		while (Step()) {
			int curLine = GetCurrentLine();

			if (m_checkBreakpoint(curLine) || curLine == line)
				break;
		}
	}
	bool ShaderDebugger::HasBreakpoint(int line)
	{
		for (int i = 0; i < m_breakpoints.size(); i++)
			if (m_breakpoints[i].Line == line)
				return true;
		return false;
	}
	void ShaderDebugger::Continue()
	{
		while (Step()) {
			if (m_checkBreakpoint(GetCurrentLine()))
				break;
		}
	}
	bool ShaderDebugger::m_checkBreakpoint(int line)
	{
		for (int i = 0; i < m_breakpoints.size(); i++) {
			if (m_breakpoints[i].Line == line) {
				if (!m_breakpoints[i].IsConditional)
					return true;
				else {
					bv_variable condRes = Immediate(m_breakpoints[i].Condition);
					bool ret = false;
					if (condRes.type == bv_type_uchar || condRes.type == bv_type_char)
						ret = bv_variable_get_uchar(condRes);
					bv_variable_deinitialize(&condRes);
					return ret;
				}
			}
		}

		return false;
	}
	void ShaderDebugger::AddBreakpoint(int line)
	{
		ClearBreakpoint(line);
		m_breakpoints.push_back(Breakpoint(line));
	}
	void ShaderDebugger::AddConditionalBreakpoint(int line, std::string condition)
	{
		ClearBreakpoint(line);
		m_breakpoints.push_back(Breakpoint(line, condition));
	}
	void ShaderDebugger::ClearBreakpoint(int line)
	{
		for (int i = 0; i < m_breakpoints.size(); i++) {
			if (m_breakpoints[i].Line == line) {
				m_breakpoints.erase(m_breakpoints.begin() + i);
				i--;
			}
		}
	}
	bool ShaderDebugger::Step()
	{
		bool done = bv_function_stepper_is_done(m_stepper);
		if (!done) {
			int curLine = m_prog->current_line;
			bool tempDone = done;

			SetDiscarded(false);

			while (curLine == m_prog->current_line && !tempDone) {
				bv_function_step(m_stepper);
				tempDone = bv_function_stepper_is_done(m_stepper);
			}

			done = tempDone;
		}

		return !done;
	}
	bool ShaderDebugger::StepOver()
	{
		std::vector<std::string> fstack = GetFunctionStack(), updatedFStack;
		
		bool done = Step();
		updatedFStack = GetFunctionStack();

		while (updatedFStack.size() > fstack.size() && done) {
			done = Step();
			updatedFStack = GetFunctionStack();
		}

		return done;
	}
	bool ShaderDebugger::StepOut()
	{
		std::string fcur = GetCurrentFunction(), updatedFunc;

		bool done = Step();
		updatedFunc = GetCurrentFunction();

		while (updatedFunc == fcur && done) {
			done = Step();
			updatedFunc = GetCurrentFunction();
		}

		return done;
	}
	bv_variable ShaderDebugger::Immediate(const std::string& src)
	{
		m_immTransl->ClearDefinitions();

		// pass on the function definitions
		const std::vector<sd::Function>& funcs = m_transl->GetFunctions();
		for (const auto& func : funcs)
			if (func.Name != m_entry)
				m_immTransl->AddFunctionDefinition(func);

		// pass on the global definitions
		const std::vector<sd::Variable>& globals = m_transl->GetGlobals();
		for (const auto& glob : globals) {
			m_immTransl->AddGlobalDefinition(glob);
			m_immTransl->AddImmediateGlobalDefinition(glob);
		}

		// pass on the local variables
		const std::vector<std::string>& locals = m_transl->GetLocals(GetCurrentFunction());
		unsigned int locIndex = 0;
		for (const auto& loc : locals) {
			sd::Variable locData;
			locData.ID = globals.size() + locIndex;
			locData.Name = loc;
			locData.Type = m_transl->GetLocalType(GetCurrentFunction(), loc);
			m_immTransl->AddGlobalDefinition(locData);
			m_immTransl->AddImmediateGlobalDefinition(locData);

			locIndex++;
		}

		bool done = m_immTransl->Parse(m_type, src, "immediate");
		std::vector<uint8_t> bytecode = m_immTransl->GetBytecode();


		if (done && m_bytecode.size() > 0) {
			bv_program* immProg = bv_program_create(bytecode.data());
			if (immProg == nullptr)
				return bv_variable_create_void(); // invalid bytecode

			// copy libraries
			bv_program_add_library(immProg, m_library);
			
			// copy all values
			const std::vector<sd::Variable>& allVariables = m_immTransl->GetGlobals();
			for (const auto& glob : allVariables) {

				bv_variable* globVal = GetLocalValue(glob.Name);
				if (globVal == nullptr)
					globVal = GetValue(glob.Name);

				bv_program_add_global(immProg, glob.Name.c_str());
				bv_program_set_global(immProg, glob.Name.c_str(), globVal == nullptr ? bv_variable_create_void() : bv_variable_copy(*globVal));
			}

			bv_function* entryPtr = bv_program_get_function(immProg, "immediate");
			if (entryPtr == nullptr) {
				bv_program_delete(immProg);
				return bv_variable_create_void(); // invalid bytecode
			}

			// call function and store its return value
			bv_variable ret = bv_variable_copy(bv_program_call(immProg, entryPtr, NULL, NULL));

			bv_program_delete(immProg);

			return ret;
		}
		else return bv_variable_create_void();

		return bv_variable_create_void();
	}

	Function ShaderDebugger::m_getFunctionInfo(const std::string& fname)
	{
		const auto& funcs = m_transl->GetFunctions();
		for (const auto& func : funcs) {
			if (func.Name == fname)
				return func;
		}

		return { 0, "", {} };
	}
}