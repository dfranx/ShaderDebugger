#pragma once
#include <vector>
#include <cstdint>
#include <glslang/MachineIndependent/localintermediate.h>

namespace sd
{
	struct ASTVariable
	{
		bool IsUniform;
		bool IsInput;
		bool IsOutput;
		bool IsBuffer;
		bool IsGlobal;
		bool IsShared;

		int Location;
		int ID;

		std::string Type;
		std::string Name;
	};

	class BVMGenerator
	{
	public:
		static void Translate(const glslang::TIntermediate& intermediate, std::vector<uint8_t>& out);
	};
}