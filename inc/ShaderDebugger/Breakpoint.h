#pragma once
#include <string>

namespace sd
{
	class Breakpoint
	{
	public:
		Breakpoint() { Line = 0; IsConditional = false; Condition = ""; }
		Breakpoint(int ln) { Line = ln; IsConditional = false; Condition = ""; }
		Breakpoint(int ln, const std::string& cond) { Line = ln; IsConditional = true; Condition = cond; }

		int Line;

		bool IsConditional;
		std::string Condition;
	};
}