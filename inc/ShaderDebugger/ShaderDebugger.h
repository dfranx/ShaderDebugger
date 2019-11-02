#pragma once
#include <ShaderDebugger/Translator.h>

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

		template<typename CodeTranslator>
		bool SetSource(sd::ShaderType stage, const std::string& src, const std::string& entry, bv_library* library)
		{
			m_entry = entry;
			m_library = library;

			CodeTranslator tr;
			bool done = tr.Parse(stage, src, entry);
			std::vector<uint8_t> bytecode = tr.GetBytecode();
			
			if (done && bytecode.size() > 0) {
				m_prog = bv_program_create(bytecode.data());
				if (m_prog == nullptr)
					return false; // invalid bytecode
				
				if (m_library != nullptr)
					bv_program_add_library(m_prog, library);
			} else return false;

			return true;
		}

		inline bv_variable Execute() { return Execute(m_entry); }
		bv_variable Execute(const std::string& func); // TODO: arguments

		bv_variable* GetValue(const std::string& gvarname);

	private:
		std::string m_entry;
		bv_library* m_library;
		bv_program* m_prog;
	};
}