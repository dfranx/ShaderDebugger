#pragma once
#include <string>
#include "Variable.h"

namespace sd
{
	struct Function
	{
		size_t ID;

		std::string Name;
		std::vector<Variable> Arguments;
	};
}