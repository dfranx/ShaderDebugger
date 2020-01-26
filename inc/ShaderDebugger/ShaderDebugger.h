#pragma once
#include <ShaderDebugger/Compiler.h>
#include <ShaderDebugger/Texture.h>
#include <ShaderDebugger/Breakpoint.h>
#include <ShaderDebugger/CommonLibrary.h>
#include <glm/glm.hpp>

#include <fstream>

extern "C" {
	#include <BlueVM.h>
}

namespace sd
{
	class ShaderDebugger
	{
	public:
		ShaderDebugger();
		~ShaderDebugger();

		template<typename CodeCompiler>
		bool SetSource(sd::ShaderType stage, const std::string& src, const std::string& entry, bv_stack* args = NULL, bv_library* library = NULL)
		{
			if (m_compiler != nullptr)
				delete m_compiler;
			if (m_immCompiler != nullptr)
				delete m_immCompiler;
			
			m_compiler = new CodeCompiler();
			m_compiler->SetImmediate(false);

			m_immCompiler = new CodeCompiler();
			m_immCompiler->SetImmediate(true);

			m_entry = entry;
			m_library = library;
			m_args = args;
			m_prog = nullptr;
			m_stepper = nullptr;
			m_discarded = false;

			m_type = stage;

			bool done = m_compiler->Parse(stage, src, entry);
			m_bytecode = m_compiler->GetBytecode();

			if (done && m_bytecode.size() > 0) {
				m_prog = bv_program_create(m_bytecode.data());
				if (m_prog == nullptr)
					return false; // invalid bytecode

				m_prog->user_data = (void*)this;
				bv_program_add_function(m_prog, "$$discard", Common::Discard);

				m_prog->property_getter = m_compiler->PropertyGetter;
					
				bv_function* entryPtr = bv_program_get_function(m_prog, entry.c_str());
				if (entryPtr == nullptr)
					return false;

				m_stepper = bv_function_stepper_create(m_prog, entryPtr, NULL, m_args);
				
				if (m_library != nullptr)
					bv_program_add_library(m_prog, library);
			} else return false;

			return true;
		}

		inline Compiler* GetCompiler() { return m_compiler; }

		inline bv_variable Execute() { return Execute(m_entry, m_args); }
		bv_variable Execute(const std::string& func, bv_stack* args = NULL); // TODO: arguments

		inline bv_variable GetReturnValue() { return bv_variable_copy(m_stepper->result); }

		std::string GetCurrentFunction();
		std::vector<std::string> GetFunctionStack();
		std::vector<std::string> GetCurrentFunctionLocals();
		bv_variable* GetLocalValue(const std::string& varname);
		int GetCurrentLine() { return m_prog->current_line; }
		void Jump(int line);
		bool Continue();
		bool Step();
		bool StepOver();
		bool StepOut();
		bool HasBreakpoint(int ln);
		void AddBreakpoint(int ln);
		void AddConditionalBreakpoint(int ln, std::string condition);
		void ClearBreakpoint(int ln);
		inline void ClearBreakpoints() { m_breakpoints.clear(); }
		bv_variable Immediate(const std::string& command);

		// for more complex types, we need to provide classType (for example, vec3 is for GLSL but float3 is for HLSL)
		// this makes ShaderDebugger work without needing to know which shader language it uses
		void SetValue(const std::string& varName, float value);
		void SetValue(const std::string& varName, const std::string& classType, glm::vec3 val);
		void SetValue(const std::string& varName, const std::string& classType, sd::Texture* val);

		bv_variable* GetValue(const std::string& gvarname);

		inline void SetDiscarded(bool d) { 
			m_discarded = d;

			if (d) {
				bv_function_stepper_abort(m_stepper);
				bv_program_abort(m_prog);
			}
		}
		inline bool IsDiscarded() { return m_discarded; }

	private:
		bool m_checkBreakpoint(int line);
		Function m_getFunctionInfo(const std::string& fname);

		bool m_discarded;

		std::vector<Breakpoint> m_breakpoints;

		sd::ShaderType m_type;
		Compiler* m_compiler, *m_immCompiler;
		std::string m_entry;
		bv_library* m_library;
		bv_program* m_prog;
		bv_stack* m_args;
		bv_function_stepper* m_stepper;
		std::vector<uint8_t> m_bytecode;
	};
}