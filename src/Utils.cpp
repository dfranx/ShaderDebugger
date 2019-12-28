#include <ShaderDebugger/Utils.h>
#include <string.h>

namespace sd
{
	glm::vec3 AsVec3(const bv_variable& var)
	{
		if (var.type == bv_type_object) {
			bv_object* obj = bv_variable_get_object(Get(var));
			if (strcmp(obj->type->name, "vec3") == 0) {
				float x = bv_variable_get_float(*bv_object_get_property(obj, "x"));
				float y = bv_variable_get_float(*bv_object_get_property(obj, "y"));
				float z = bv_variable_get_float(*bv_object_get_property(obj, "z"));

				return glm::vec3(x,y,z);
			}
			// TODO: vec2, etc...
		}

		return glm::vec3(0.0f);
	}
	float AsFloat(const bv_variable& var)
	{
		if (var.type == bv_type_float)
			return bv_variable_get_float(Get(var));
		return 0.0f;
	}
	int AsInteger(const bv_variable& var)
	{
		if (var.type == bv_type_int)
			return bv_variable_get_int(Get(var));
		return 0;
	}
	bv_variable Get(const bv_variable& var)
	{
		if (var.type == bv_type_pointer)
			return Get(*(bv_variable*)var.value);

		return var;
	}
	bv_variable* GetPtr(bv_variable* var)
	{
		if (var->type == bv_type_pointer)
			return GetPtr((bv_variable*)var->value);

		return var;
	}
}