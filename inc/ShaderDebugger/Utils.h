#pragma once

#include <glm/glm.hpp>

extern "C" {
	#include <BlueVM.h>
}

namespace sd
{
	glm::vec3 AsVec3(const bv_variable& var);
	float AsFloat(const bv_variable& var);
	int AsInteger(const bv_variable& var);
	bv_variable Get(const bv_variable& var);
	bv_variable* GetPtr(bv_variable* var);
}