#pragma once

#include <ShaderDebugger/Matrix.h>
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
		else if (var.type == bv_type_int)
			ret[0] = bv_variable_get_int(var);
		else if (var.type == bv_type_uint)
			ret[0] = bv_variable_get_uint(var);
		else if (var.type == bv_type_uchar)
			ret[0] = bv_variable_get_uchar(var);
		else if (var.type == bv_type_float)
			ret[0] = bv_variable_get_float(var);

		return ret;
	}
	sd::Matrix AsMatrix(const bv_variable& var);
	float AsFloat(const bv_variable& var);
	int AsInteger(const bv_variable& var);
	unsigned int AsUnsignedInteger(const bv_variable& var);
	unsigned int AsBool(const bv_variable& var);

	bv_type GetVectorTypeFromName(const char* name);

	void GetMatrixSizeFromName(const char* name, int* cols, int* rows);
	bv_type GetMatrixTypeFromName(const char* name);

	bool IsBasicTexture(const char* name);
	u8 GetTextureDimension(const char* name);

	sd::Matrix* CopyMatrixData(sd::Matrix* mat);
}