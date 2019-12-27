#include <ShaderDebugger/GLSLLibrary.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

namespace sd
{
	namespace Library
	{
		/* vec3() */
		bv_variable lib_vec3_constructor(bv_object* obj, u8 count, bv_variable* args)
		{
			if (count == 0) { // vec3()
				bv_object_set_property(obj, "x", bv_variable_create_float(0.0f));
				bv_object_set_property(obj, "y", bv_variable_create_float(0.0f));
				bv_object_set_property(obj, "z", bv_variable_create_float(0.0f));
			} else if (count == 1) { // vec3(0.5f)
				bv_object_set_property(obj, "x", bv_variable_cast(bv_type_float, args[0]));
				bv_object_set_property(obj, "y", bv_variable_cast(bv_type_float, args[0])); // TODO: do just one cast
				bv_object_set_property(obj, "z", bv_variable_cast(bv_type_float, args[0]));
			} else if (count == 3) { // vec3(2, true, 1.0f)
				bv_object_set_property(obj, "x", bv_variable_cast(bv_type_float, args[0]));
				bv_object_set_property(obj, "y", bv_variable_cast(bv_type_float, args[1]));
				bv_object_set_property(obj, "z", bv_variable_cast(bv_type_float, args[2]));
			}

			return bv_variable_create_void();
		}

		/* vec3() + ... */
		bv_variable lib_vec3_operator_add(bv_object* obj, u8 count, bv_variable* args)
		{
			float myX = bv_variable_get_float(*bv_object_get_property(obj, "x"));
			float myY = bv_variable_get_float(*bv_object_get_property(obj, "y"));
			float myZ = bv_variable_get_float(*bv_object_get_property(obj, "z"));

			bv_variable ret = bv_variable_create_object(obj->type);
			bv_object* retObj = bv_variable_get_object(ret);

			if (count == 1) {
				// vec3 + vec3
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					if (strcmp(vec->type->name, "vec3") == 0) {
						float vecX = bv_variable_get_float(*bv_object_get_property(vec, "x"));
						float vecY = bv_variable_get_float(*bv_object_get_property(vec, "y"));
						float vecZ = bv_variable_get_float(*bv_object_get_property(vec, "z"));

						bv_object_set_property(retObj, "x", bv_variable_create_float(myX + vecX));
						bv_object_set_property(retObj, "y", bv_variable_create_float(myY + vecY));
						bv_object_set_property(retObj, "z", bv_variable_create_float(myZ + vecZ));
					}
				}

				// vec3 + scalar
				else {
					bv_variable temp = bv_variable_cast(bv_type_float, args[0]);
					float sVal = bv_variable_get_float(temp);

					bv_object_set_property(retObj, "x", bv_variable_create_float(myX + sVal));
					bv_object_set_property(retObj, "y", bv_variable_create_float(myY + sVal));
					bv_object_set_property(retObj, "z", bv_variable_create_float(myZ + sVal));

					bv_variable_deinitialize(&temp);
				}
			}

			return ret;
		}
		bv_variable lib_vec3_operator_equal(bv_object* obj, u8 count, bv_variable* args)
		{
			float myX = bv_variable_get_float(*bv_object_get_property(obj, "x"));
			float myY = bv_variable_get_float(*bv_object_get_property(obj, "y"));
			float myZ = bv_variable_get_float(*bv_object_get_property(obj, "z"));

			bv_variable ret = bv_variable_create_uchar(0);

			if (count == 1) {
				// vec3 == vec3
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					if (vec != nullptr) {
						if (strcmp(vec->type->name, "vec3") == 0) {
							float vecX = bv_variable_get_float(*bv_object_get_property(vec, "x"));
							float vecY = bv_variable_get_float(*bv_object_get_property(vec, "y"));
							float vecZ = bv_variable_get_float(*bv_object_get_property(vec, "z"));

							bv_variable_set_uchar(&ret, myX == vecX && myY == vecY && myZ == vecZ);
						}
					}
				}

				// vec3 + scalar
				else {
					bv_variable temp = bv_variable_cast(bv_type_float, args[0]);
					float sVal = bv_variable_get_float(temp);

					bv_variable_set_uchar(&ret, myX == sVal && myY == sVal && myZ == sVal);

					bv_variable_deinitialize(&temp);
				}
			}

			return ret;
		}

		/* floor() */
		bv_variable lib_floor(u8 count, bv_variable* args)
		{
			if (count == 1) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_variable ret = bv_variable_copy(args[0]); // modify a copy of the argument and return it
					bv_object* vec = bv_variable_get_object(ret);

					if (strcmp(vec->type->name, "vec3") == 0) {
						float xVal = bv_variable_get_float(*bv_object_get_property(vec, "x"));
						float yVal = bv_variable_get_float(*bv_object_get_property(vec, "y"));
						float zVal = bv_variable_get_float(*bv_object_get_property(vec, "z"));

						bv_object_set_property(vec, "x", bv_variable_create_float(floor(xVal)));
						bv_object_set_property(vec, "y", bv_variable_create_float(floor(yVal)));
						bv_object_set_property(vec, "z", bv_variable_create_float(floor(zVal)));
					} /* else if vec4, vec2 ... */

					return ret;
				} else {
					bv_variable ret = bv_variable_cast(bv_type_float, args[0]);
					bv_variable_set_float(&ret, floor(bv_variable_get_float(ret)));
					return ret;
				}
			}
			
			return bv_variable_create_float(0.0f); // floor() must have 1 argument!
		}

		bv_library* GLSL()
		{
			bv_library* lib = bv_library_create();

			// vec3 implementation
			bv_object_info* vec3 = bv_object_info_create("vec3");
			bv_object_info_add_property(vec3, "x");
			bv_object_info_add_property(vec3, "y");
			bv_object_info_add_property(vec3, "z");
			bv_object_info_add_ext_method(vec3, "vec3", lib_vec3_constructor);
			bv_object_info_add_ext_method(vec3, "==", lib_vec3_operator_equal);
			bv_object_info_add_ext_method(vec3, "+", lib_vec3_operator_add);
			bv_library_add_object_info(lib, vec3);

			// floor()
			bv_library_add_function(lib, "floor", lib_floor);

			return lib;
		}
	}
}