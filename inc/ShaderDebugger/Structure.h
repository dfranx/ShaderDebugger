#pragma once
#include <vector>
#include <ShaderDebugger/Variable.h>

namespace sd
{
	struct Structure
	{
		size_t ID;

		std::string Name;
		std::vector<Variable> Members;
	};
}