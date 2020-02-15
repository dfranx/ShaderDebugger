#include <ShaderDebugger/ShaderDebugger.h>
#include <ShaderDebugger/Utils.h>
#include <assert.h>

namespace sd
{
	ShaderDebugger::ShaderDebugger()
	{
		m_prog = nullptr;
		m_library = nullptr;
		m_stepper = nullptr;
		m_compiler = nullptr;
		m_discarded = false;
		m_immCompiler = nullptr;
		m_args = nullptr;
		m_type = sd::ShaderType::Vertex;
	}
	ShaderDebugger::~ShaderDebugger()
	{
		m_clear();
	}
	bv_variable ShaderDebugger::Execute(const std::string& func, bv_stack* args)
	{
		assert(m_prog != nullptr);

		bv_function* funcPtr = bv_program_get_function(m_prog, func.c_str());
		if (funcPtr == nullptr)
			return bv_variable_create_void(); // function doesn't exist

		SetDiscarded(false);

		// call function and store its return value
		return bv_program_call(m_prog, funcPtr, args, NULL);
	}
	void ShaderDebugger::AddGlobal(const std::string& varName)
	{
		assert(m_prog != nullptr);

		bv_program_add_global(m_prog, varName.c_str());
	}
	bv_variable* ShaderDebugger::GetGlobalValue(const std::string& gvarname)
	{
		assert(m_prog != nullptr);

		return bv_program_get_global(m_prog, const_cast<char*>(gvarname.c_str())); // TODO: why does get_global need non const???
	}
	void ShaderDebugger::SetSemanticValue(const std::string& name, bv_variable var)
	{
		std::string lName = name;
		std::transform(lName.begin(), lName.end(), lName.begin(), ::tolower);
		
		if (m_semantics.count(lName))
			bv_variable_deinitialize(&m_semantics[lName]);
		
		m_semantics[lName] = bv_variable_copy(var);
	}
	bv_variable ShaderDebugger::GetSemanticValue(const std::string& name)
	{
		std::string lName = name;
		std::transform(lName.begin(), lName.end(), lName.begin(), ::tolower);
		return ((m_semantics.count(lName) > 0) ? m_semantics[lName] : bv_variable_create_void());
	}
	void ShaderDebugger::SetGlobalValue(const std::string& varName, bv_variable value)
	{
		bv_variable* glob = bv_program_get_global(m_prog, varName.c_str());
		if (glob != nullptr) {
			bv_variable_deinitialize(glob);
			*glob = bv_variable_copy(value); // don't take control of user allocate memory, rather copy it
		}
	}
	void ShaderDebugger::SetGlobalValue(const std::string& varName, float value)
	{
		assert(m_prog != nullptr);

		bv_program_set_global(m_prog, varName.c_str(), bv_variable_create_float(value));
	}
	bool ShaderDebugger::SetGlobalValue(const std::string& varName, const std::string& classType, glm::vec4 val)
	{
		assert(m_prog != nullptr);

		bv_object_info* objInfo = bv_program_get_object_info(m_prog, classType.c_str());
		if (objInfo == nullptr) {
			m_error = "The object definition for \'" + classType + "\' doesn't exist.";
			return false;
		}

		bv_variable objVar = bv_variable_create_object(objInfo);
		bv_object* obj = bv_variable_get_object(objVar);

		bv_type vecType = sd::GetVectorTypeFromName(classType.c_str());
		u16 cCount = objInfo->props.name_count;

		for (u16 i = 0; i < cCount; i++)
			obj->prop[i] = bv_variable_cast(vecType, bv_variable_create_float(val[i]));

		bv_program_set_global(m_prog, varName.c_str(), objVar);

		return true;
	}
	bool ShaderDebugger::SetGlobalValue(const std::string& varName, const std::string& classType, sd::Texture* val)
	{
		assert(m_prog != nullptr);

		bv_object_info* objInfo = bv_program_get_object_info(m_prog, classType.c_str());
		if (objInfo == nullptr) {
			m_error = "The object definition for \'" + classType + "\' doesn't exist.";
			return false;
		}

		bv_variable objVar = bv_variable_create_object(objInfo);
		bv_object* obj = bv_variable_get_object(objVar);

		obj->user_data = (void*)val;

		bv_program_set_global(m_prog, varName.c_str(), objVar);

		return true;
	}
	bool ShaderDebugger::SetGlobalValue(const std::string& varName, const std::string& classType, glm::mat4 val)
	{
		int cols = 0, rows = 0;
		sd::GetMatrixSizeFromName(classType.c_str(), &cols, &rows);

		if (cols != 0 && rows != 0) {
			bv_variable* glob = bv_program_get_global(m_prog, varName.c_str());
			if (glob != nullptr) {
				bv_variable_deinitialize(glob);
				*glob = Common::create_mat(m_prog, classType.c_str(), new sd::Matrix(val, cols, rows)); // I know that passing pointers and then deleting them somewhere else is bad practice...

				return true;
			}
		}

		return false;
	}

	void ShaderDebugger::SetArguments(bv_stack* args)
	{
		assert(m_prog != nullptr);

		if (m_stepper != nullptr)
			bv_function_stepper_delete(m_stepper);

		m_args = args;

		bv_function* entryPtr = bv_program_get_function(m_prog, m_entry.c_str());
		if (entryPtr == nullptr)
			return;

		m_stepper = bv_function_stepper_create(m_prog, entryPtr, NULL, m_args);
	}

	std::string ShaderDebugger::GetCurrentFunction()
	{
		assert(m_stepper != nullptr);

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
		assert(m_stepper != nullptr);

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
		assert(m_compiler != nullptr);

		std::string curFunc = GetCurrentFunction();
		if (curFunc.size() == 0)
			return std::vector<std::string>();
		return m_compiler->GetLocals(curFunc);
	}
	bv_variable* ShaderDebugger::GetLocalValue(const std::string& varname)
	{
		assert(m_compiler != nullptr);
		assert(m_stepper != nullptr);

		bv_scope* scope = m_stepper->scope;
		std::string curFunc = GetCurrentFunction();
		const auto& locList = m_compiler->GetLocals(curFunc);
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
	bool ShaderDebugger::Continue()
	{
		while (Step()) {
			if (m_checkBreakpoint(GetCurrentLine()))
				return true;
		}
		return false;
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
					if (bv_type_is_integer(condRes.type))
						ret = bv_variable_get_int(condRes);
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
		assert(m_stepper != nullptr);

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
			if (m_checkBreakpoint(GetCurrentLine()))
				return done;

			done = Step();
			updatedFStack = GetFunctionStack();
		}

		return done;
	}
	bool ShaderDebugger::StepOut()
	{
		std::vector<std::string> fstack = GetFunctionStack(), updatedFStack;

		bool done = Step();
		updatedFStack = GetFunctionStack();

		while (updatedFStack.size() >= fstack.size() && done) {
			if (m_checkBreakpoint(GetCurrentLine()))
				return done;

			done = Step();
			updatedFStack = GetFunctionStack();
		}

		return done;
	}
	bv_variable ShaderDebugger::Immediate(const std::string& src)
	{
		assert(m_immCompiler != nullptr);

		m_immCompiler->ClearDefinitions();

		// pass string table
		const std::vector<std::string>& strtable = m_compiler->GetStringTable();
		for (const std::string& str : strtable)
			m_immCompiler->AddStringTableEntry(str);

		// pass the function definitions
		const std::vector<sd::Function>& funcs = m_compiler->GetFunctions();
		for (const auto& func : funcs)
			if (func.Name != m_entry)
				m_immCompiler->AddFunctionDefinition(func);

		// pass the structure definitions
		const std::vector<sd::Structure>& structs = m_compiler->GetStructures();
		for (const auto& str : structs)
			m_immCompiler->AddStructureDefinition(str);

		// pass the global definitions
		const std::vector<sd::Variable>& globals = m_compiler->GetGlobals();
		for (const auto& glob : globals) {
			m_immCompiler->AddGlobalDefinition(glob);
			m_immCompiler->AddImmediateGlobalDefinition(glob);
		}

		// pass macro definitions
		pp::MacroMap macros = m_compiler->GetMacroList();
		for (const auto& m : macros)
			m_immCompiler->AddMacro(m.first, m.second);

		// pass arguments
		std::string curFunc = GetCurrentFunction();
		unsigned int locIndex = 0;
		for (const auto& f : funcs) {
			if (f.Name == curFunc) {

				for (const auto& arg : f.Arguments) {
					sd::Variable locData = arg;
					locData.ID = globals.size() + locIndex;
					m_immCompiler->AddGlobalDefinition(locData);
					m_immCompiler->AddImmediateGlobalDefinition(locData);

					locIndex++;
				}

				break;
			}
		}

		// pass on the local variables
		const std::vector<std::string>& locals = m_compiler->GetLocals(GetCurrentFunction());
		for (const auto& loc : locals) {
			sd::Variable locData;
			locData.ID = globals.size() + locIndex;
			locData.Name = loc;
			locData.Type = m_compiler->GetLocalType(GetCurrentFunction(), loc);
			m_immCompiler->AddGlobalDefinition(locData);
			m_immCompiler->AddImmediateGlobalDefinition(locData);

			locIndex++;
		}

		bool done = m_immCompiler->Parse(m_type, src, "immediate");
		std::vector<uint8_t> bytecode = m_immCompiler->GetBytecode();


		if (done && m_bytecode.size() > 0) {
			bv_program* immProg = bv_program_create(bytecode.data());
			if (immProg == nullptr)
				return bv_variable_create_void(); // invalid bytecode

			for (u16 i = 0; i < m_prog->block->functions->count; i++)
				bv_program_add_function_pointer(immProg, m_prog->block->functions->names[i], m_prog->functions[i]);
			
			// property getter
			immProg->property_getter = m_immCompiler->PropertyGetter;
			immProg->default_constructor = m_immCompiler->ObjectConstructor;

			// copy libraries
			bv_program_add_library(immProg, m_library);
			immProg->user_data = m_prog->user_data;
			
			// copy all values
			const std::vector<sd::Variable>& allVariables = m_immCompiler->GetGlobals();
			for (const auto& glob : allVariables) {

				bv_variable* globVal = GetLocalValue(glob.Name);
				if (globVal == nullptr)
					globVal = GetGlobalValue(glob.Name);

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
		const auto& funcs = m_compiler->GetFunctions();
		for (const auto& func : funcs) {
			if (func.Name == fname)
				return func;
		}

		return { 0, "", {} };
	}
	void ShaderDebugger::m_clear()
	{
		if (m_compiler != nullptr)
			delete m_compiler;
		if (m_immCompiler != nullptr)
			delete m_immCompiler;
		if (m_stepper != nullptr) {
			bv_function_stepper_delete(m_stepper);
			m_stepper = nullptr;
		}
		if (m_prog != nullptr) {
			bv_program_delete(m_prog);
			m_prog = nullptr;
		}
	}
}