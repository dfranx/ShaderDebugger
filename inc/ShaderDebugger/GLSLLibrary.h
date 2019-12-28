#pragma once

extern "C" {
	#include <BlueVM.h>
}

namespace sd
{
	namespace Library
	{
		bv_library* GLSL();
		bv_variable GLSLswizzle(bv_program* prog, bv_object* obj, const bv_string field);
	}
}