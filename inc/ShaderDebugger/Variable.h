#pragma once
#include <string>
#include <unordered_map>

namespace sd
{
	struct Variable
	{
		size_t ID;

		size_t InputSlot;

		std::string Name;
		std::string Type;
		bool Smooth, Flat, NoPerspective;
	};
}