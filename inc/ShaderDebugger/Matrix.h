#pragma once
#include <glm/glm.hpp>

namespace sd
{
	class Matrix
	{
	public:
		glm::mat4 Data;
		int Columns;
		int Rows;
		unsigned char Type; // bv_type
	};
}