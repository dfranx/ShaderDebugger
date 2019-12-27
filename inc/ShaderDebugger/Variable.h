#pragma once
#include <string>
#include <unordered_map>

namespace sd
{
	class Variable
	{
	public:
		Variable()
		{
			ID = InputSlot = 0;
			Name = Type = "";
			Smooth = Flat = NoPerspective = false;
			Storage = StorageType::None;
		}

		enum class StorageType
		{
			None,
			Constant,
			In,
			Out,
			Attribute,
			Uniform,
			Varying,
			Buffer,
			Shared
		} Storage;

		size_t ID;

		size_t InputSlot;

		std::string Name;
		std::string Type;
		bool Smooth, Flat, NoPerspective;
	};
}