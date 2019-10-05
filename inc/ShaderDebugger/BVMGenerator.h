#pragma once
#include <vector>
#include <cstdint>
#include <glslang/MachineIndependent/localintermediate.h>

namespace sd
{
	class BVMGenerator
	{
	public:
		static void Translate(const glslang::TIntermediate& intermediate, std::vector<uint8_t>& out);
	};
}