#include <ShaderDebugger/Utils.h>
#include <string.h>

namespace sd
{
	float AsFloat(const bv_variable& var)
	{
		if (var.type == bv_type_float)
			return bv_variable_get_float(var);
		return 0.0f;
	}
	int AsInteger(const bv_variable& var)
	{
		if (var.type == bv_type_int)
			return bv_variable_get_int(var);
		return 0;
	}

	u8 GetVectorSizeFromName(const char* name)
	{
		static const char* names3[] = { "ivec3", "uvec3", "bvec3", "dvec3", "vec3" };
		static const char* names2[] = { "ivec2", "uvec2", "bvec2", "dvec2", "vec2" };

		for (u8 i = 0; i < 5; i++) {
			if (strcmp(name, names3[i]) == 0) return 3;
			if (strcmp(name, names2[i]) == 0) return 2;
		}

		return 4;
	}
	bv_type GetVectorTypeFromName(const char* name)
	{
		static const char* iNames[] = { "ivec4", "ivec3", "ivec2" };
		static const char* uNames[] = { "uvec4", "uvec3", "uvec2" };
		static const char* bNames[] = { "bvec4", "bvec3", "bvec2" };

		for (u8 i = 0; i < 3; i++) {
			if (strcmp(name, iNames[i]) == 0) return bv_type_int;
			if (strcmp(name, uNames[i]) == 0) return bv_type_uint;
			if (strcmp(name, bNames[i]) == 0) return bv_type_uchar;
		}

		return bv_type_float;
	}
}