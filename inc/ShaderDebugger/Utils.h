#pragma once

#include <glm/glm.hpp>

extern "C" {
	#include <BlueVM.h>
}

namespace sd
{
	template <size_t c, typename t>
	glm::vec<c, t, glm::defaultp> AsVector(const bv_variable& var)
	{
		glm::vec<c, t, glm::defaultp> ret(0);

		if (var.type == bv_type_object) {
			bv_object* obj = bv_variable_get_object(var);
			bv_type type = obj->prop[0].type;

			if (type == bv_type_int)
				for (u16 i = 0; i < obj->type->props.name_count; i++)
					ret[i] = bv_variable_get_int(obj->prop[i]);
			else if (type == bv_type_uint)
				for (u16 i = 0; i < obj->type->props.name_count; i++)
					ret[i] = bv_variable_get_uint(obj->prop[i]);
			else if (type == bv_type_uchar)
				for (u16 i = 0; i < obj->type->props.name_count; i++)
					ret[i] = bv_variable_get_uchar(obj->prop[i]);
			else
				for (u16 i = 0; i < obj->type->props.name_count; i++)
					ret[i] = bv_variable_get_float(obj->prop[i]);
		}

		return ret;
	}
	float AsFloat(const bv_variable& var);
	int AsInteger(const bv_variable& var);

	u8 GetVectorSizeFromName(const char* name);
	bv_type GetVectorTypeFromName(const char* name);
}