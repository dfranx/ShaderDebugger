#pragma once
#include <string>
#include <ShaderDebugger/Variable.h>

namespace sd
{
	struct Function
	{
		size_t ID;

		std::string Name;
		std::string ReturnType;
		std::vector<Variable> Arguments;
	};
}