#pragma once

#include <glm/glm.hpp>

extern "C" {
	#include <BlueVM.h>
}

namespace sd
{
	glm::vec3 AsVec3(const bv_variable& var);
}