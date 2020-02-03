#pragma once
#include <glm/glm.hpp>

extern "C" {
	#include <BlueVM/bv_type.h>
}

namespace sd
{
	class Matrix
	{
	public:
		Matrix() { Data = glm::mat4(0.0f); Columns = 4; Rows = 4; Type = bv_type_float; }
		Matrix(glm::mat4 data, int cols, int rows) { Data = data; Columns = cols; Rows = rows; Type = bv_type_float; }
		glm::mat4 Data;
		int Columns;
		int Rows;
		unsigned char Type; // bv_type
	};
}