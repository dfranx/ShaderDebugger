#include <ShaderDebugger/GLSLLibrary.h>
#include <ShaderDebugger/Texture.h>
#include <ShaderDebugger/Utils.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

namespace sd
{
	namespace Library
	{
		// helper functions:
		bv_variable create_vec3(bv_program* prog, glm::vec3 val = glm::vec3(0.0f))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "vec3"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_float(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_float(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_float(val.z));

			return ret;
		}
		bv_variable create_ivec3(bv_program* prog, glm::ivec3 val = glm::ivec3(0))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "ivec3"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_int(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_int(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_int(val.z));

			return ret;
		}
		bv_variable create_uvec3(bv_program* prog, glm::uvec3 val = glm::uvec3(0u))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "uvec3"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uint(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uint(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_uint(val.z));

			return ret;
		}
		bv_variable create_bvec3(bv_program* prog, glm::bvec3 val = glm::bvec3(false))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "bvec3"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uchar(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uchar(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_uchar(val.z));

			return ret;
		}
		
		bv_variable create_vec4(bv_program* prog, glm::vec4 val = glm::vec4(0.0f))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "vec4"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_float(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_float(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_float(val.z));
			bv_object_set_property(retObj, "w", bv_variable_create_float(val.w));

			return ret;
		}
		bv_variable create_ivec4(bv_program* prog, glm::ivec4 val = glm::ivec4(0))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "ivec4"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_int(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_int(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_int(val.z));
			bv_object_set_property(retObj, "w", bv_variable_create_int(val.w));

			return ret;
		}
		bv_variable create_uvec4(bv_program* prog, glm::uvec4 val = glm::uvec4(0u))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "uvec4"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uint(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uint(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_uint(val.z));
			bv_object_set_property(retObj, "w", bv_variable_create_uint(val.w));

			return ret;
		}
		bv_variable create_bvec4(bv_program* prog, glm::bvec4 val = glm::bvec4(false))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "bvec4"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uchar(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uchar(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_uchar(val.z));
			bv_object_set_property(retObj, "w", bv_variable_create_uchar(val.w));

			return ret;
		}

		bv_variable create_vec2(bv_program* prog, glm::vec2 val = glm::vec2(0.0f))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "vec2"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_float(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_float(val.y));

			return ret;
		}
		bv_variable create_ivec2(bv_program* prog, glm::ivec2 val = glm::ivec2(0))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "ivec2"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_int(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_int(val.y));

			return ret;
		}
		bv_variable create_uvec2(bv_program* prog, glm::uvec2 val = glm::uvec2(0u))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "uvec2"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uint(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uint(val.y));

			return ret;
		}
		bv_variable create_bvec2(bv_program* prog, glm::bvec2 val = glm::bvec2(false))
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, "bvec2"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uchar(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uchar(val.y));

			return ret;
		}

		bv_variable create_vec(bv_program* prog, bv_type type, u16 components)
		{
			if (components == 3) // 3D vector
			{
				if (type == bv_type_uint)
					return create_uvec3(prog);
				else if (type == bv_type_int)
					return create_ivec3(prog);
				else if (type == bv_type_uchar)
					return create_bvec3(prog);
				else
					return create_vec3(prog);
			}
			else if (components == 4) // 4D vector
			{
				if (type == bv_type_uint)
					return create_uvec4(prog);
				else if (type == bv_type_int)
					return create_ivec4(prog);
				else if (type == bv_type_uchar)
					return create_bvec4(prog);
				else
					return create_vec4(prog);
			}
			else if (components == 2) // 2D vector
			{
				if (type == bv_type_uint)
					return create_uvec2(prog);
				else if (type == bv_type_int)
					return create_ivec2(prog);
				else if (type == bv_type_uchar)
					return create_bvec2(prog);
				else
					return create_vec2(prog);
			}

			return create_vec3(prog);
		}
		bv_type merge_type(bv_type type1, bv_type type2)
		{
			if (type1 == bv_type_float || type2 == bv_type_float)
				return bv_type_float;
			else if (type1 == bv_type_uint || type2 == bv_type_uint)
				return bv_type_uint;
			else if (type1 == bv_type_int || type2 == bv_type_int)
				return bv_type_int;
			else if (type1 == bv_type_ushort || type2 == bv_type_ushort)
				return bv_type_ushort;
			else if (type1 == bv_type_short || type2 == bv_type_short)
				return bv_type_short;
			else if (type1 == bv_type_uchar || type2 == bv_type_uchar)
				return bv_type_uchar;
			else if (type1 == bv_type_char || type2 == bv_type_char)
				return bv_type_char;

			return type1;
		}


		/* swizzle */
		bv_variable GLSLswizzle(bv_program* prog, bv_object* obj, const bv_string field)
		{
			if (strcmp(obj->type->name, "vec3") == 0) {
				const char* rgba = "rgba\0";
				const char* xyzw = "xyzw\0";
				const char* stpq = "stpq\0";

				char name[4][2] = { 0 };
				for (int j = 0; j < strlen(field); j++)
					for (int i = 0; i < 4; i++) {
						if (field[j] == rgba[i])
							name[j][0] = xyzw[i];
						else if (field[j] == stpq[i])
							name[j][0] = xyzw[i];
						else if (field[j] == xyzw[i])
							name[j][0] = xyzw[i];
					}

				printf("[DEBUG] swizzle: %s\n", field);

				if (strlen(field) == 1)
					return bv_variable_create_pointer(bv_object_get_property(obj, name[0]));
				else if (strlen(field) == 2) {
					bv_variable ret = create_vec(prog, obj->prop[0].type, 2);
					bv_object* vec = bv_variable_get_object(ret);
					bv_object_set_property(vec, "x", bv_variable_create_pointer(bv_object_get_property(obj, name[0])));
					bv_object_set_property(vec, "y", bv_variable_create_pointer(bv_object_get_property(obj, name[1])));
					return ret;
				}
				else if (strlen(field) == 3) {
					bv_variable ret = create_vec(prog, obj->prop[0].type, 3);
					bv_object* vec = bv_variable_get_object(ret);
					bv_object_set_property(vec, "x", bv_variable_create_pointer(bv_object_get_property(obj, name[0])));
					bv_object_set_property(vec, "y", bv_variable_create_pointer(bv_object_get_property(obj, name[1])));
					bv_object_set_property(vec, "z", bv_variable_create_pointer(bv_object_get_property(obj, name[2])));
					return ret;
				}
				else if (strlen(field) == 4) {
					bv_variable ret = create_vec(prog, obj->prop[0].type, 4);
					bv_object* vec = bv_variable_get_object(ret);
					bv_object_set_property(vec, "x", bv_variable_create_pointer(bv_object_get_property(obj, name[0])));
					bv_object_set_property(vec, "y", bv_variable_create_pointer(bv_object_get_property(obj, name[1])));
					bv_object_set_property(vec, "z", bv_variable_create_pointer(bv_object_get_property(obj, name[2])));
					bv_object_set_property(vec, "4", bv_variable_create_pointer(bv_object_get_property(obj, name[3])));
					return ret;
				}
			}
		}

		/* operators for vectors: ==, +, -, >, /, *, ++, --, ! */
		bv_variable lib_glsl_vec_operator_equal(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_uchar(0);

			if (count == 1) {
				// vec == vec
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					if (vec != nullptr) {
						bv_variable_set_uchar(&ret, 1);
						for (u16 i = 0; i < me->type->props.name_count; i++) {
							u8 eq = bv_variable_op_equal(prog, me->prop[i], vec->prop[i]);
							bv_variable_set_uchar(&ret, eq && bv_variable_get_uchar(ret));
						}
					}
				}
			}

			return ret;
		}
		bv_variable lib_glsl_vec_operator_add(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_variable ret;

			if (count == 1) {
				// vec + vec
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					bv_type mtype = merge_type(me->prop[0].type, vec->prop[0].type);

					ret = create_vec(prog, mtype, me->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < me->type->props.name_count; i++) {
						bv_variable_deinitialize(&retObj->prop[i]);
						retObj->prop[i] = bv_variable_cast(mtype, bv_variable_op_add(prog, me->prop[i], vec->prop[i]));
					}
				}
			}

			return ret;
		}
		bv_variable lib_glsl_vec_operator_minus(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_variable ret;

			if (count == 1) {
				// vec - vec
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					bv_type mtype = merge_type(me->prop[0].type, vec->prop[0].type);

					ret = create_vec(prog, mtype, me->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < me->type->props.name_count; i++) {
						bv_variable_deinitialize(&retObj->prop[i]);
						retObj->prop[i] = bv_variable_cast(mtype, bv_variable_op_subtract(prog, me->prop[i], vec->prop[i]));
					}
				}
			}
			else if (count == 0) {
				ret = bv_variable_create_object(me->type);

				bv_object* retObj = bv_variable_get_object(ret);

				for (u16 i = 0; i < me->type->props.name_count; i++) {
					bv_variable_deinitialize(&retObj->prop[i]);
					retObj->prop[i] = bv_variable_op_negate(prog, me->prop[i]);
				}
			}

			return ret;
		}
		bv_variable lib_glsl_vec_operator_greater(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_uchar(0);

			if (count == 1) {
				// vec == vec
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					if (vec != nullptr) {
						bv_variable_set_uchar(&ret, 1);
						for (u16 i = 0; i < me->type->props.name_count; i++) {
							u8 eq = bv_variable_op_greater_than(prog, me->prop[i], vec->prop[i]);
							bv_variable_set_uchar(&ret, eq && bv_variable_get_uchar(ret));
						}
					}
				}
			}

			return ret;
		}
		bv_variable lib_glsl_vec_operator_divide(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_variable ret;

			if (count == 1) {
				// vec / vec
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					bv_type mtype = merge_type(me->prop[0].type, vec->prop[0].type);

					ret = create_vec(prog, mtype, me->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < me->type->props.name_count; i++) {
						bv_variable_deinitialize(&retObj->prop[i]);
						retObj->prop[i] = bv_variable_cast(mtype, bv_variable_op_divide(prog, me->prop[i], vec->prop[i]));
					}
				}
			}

			return ret;
		}
		bv_variable lib_glsl_vec_operator_multiply(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_variable ret;

			if (count == 1) {
				// vec / vec
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					bv_type mtype = merge_type(me->prop[0].type, vec->prop[0].type);

					ret = create_vec(prog, mtype, me->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < me->type->props.name_count; i++) {
						bv_variable_deinitialize(&retObj->prop[i]);
						retObj->prop[i] = bv_variable_cast(mtype, bv_variable_op_multiply(prog, me->prop[i], vec->prop[i]));
					}
				}
			}

			return ret;
		}
		bv_variable lib_glsl_vec_operator_increment(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			if (count == 0) {
				for (u16 i = 0; i < me->type->props.name_count; i++)
					bv_variable_op_increment(prog, &me->prop[i]);
			}

			return bv_variable_create_void();
		}
		bv_variable lib_glsl_vec_operator_decrement(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			if (count == 0) {
				for (u16 i = 0; i < me->type->props.name_count; i++)
					bv_variable_op_decrement(prog, &me->prop[i]);
			}

			return bv_variable_create_void();
		}
		bv_variable lib_glsl_vec_operator_logical_not(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();

			if (count == 0) {
				ret = bv_variable_create_object(me->type);

				bv_object* retObj = bv_variable_get_object(ret);

				for (u16 i = 0; i < me->type->props.name_count; i++) {
					bv_variable_deinitialize(&retObj->prop[i]);
					retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_not(prog, me->prop[i]));
				}
			}

			return bv_variable_create_void();
		}

		/* vec() */
		bv_variable lib_glsl_vec_constructor(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_type type = sd::GetVectorTypeFromName(me->type->name);
			u8 size = sd::GetVectorSizeFromName(me->type->name);

			if (count == 0) { // gvecX()
				for (u16 i = 0; i < me->type->props.name_count; i++)
					me->prop[i] = bv_variable_cast(type, bv_variable_create_float(0.0f));
			}
			else if (count == 1) {
				if (args[0].type == bv_type_object) { // gvecX(fvecX)
					bv_object* vec = bv_variable_get_object(args[0]);

					for (u16 i = 0; i < me->type->props.name_count; i++)
						me->prop[i] = bv_variable_cast(type, vec->prop[i]);
				}
				else { // gvecX(scalar)
					for (u16 i = 0; i < me->type->props.name_count; i++)
						me->prop[i] = bv_variable_cast(type, args[0]);
				}
			}
			else if (count == size) { // vec3(2, true, 1.0f)
				for (u16 i = 0; i < me->type->props.name_count; i++)
					me->prop[i] = bv_variable_cast(type, args[i]);
			}
			else if (count == 2 && size == 4) { // vec4(vec2, vec2)
				bv_object* vec1 = bv_variable_get_object(args[0]);
				bv_object* vec2 = bv_variable_get_object(args[1]);

				me->prop[0] = bv_variable_cast(type, vec1->prop[0]);
				me->prop[1] = bv_variable_cast(type, vec1->prop[1]);
				me->prop[2] = bv_variable_cast(type, vec2->prop[0]);
				me->prop[3] = bv_variable_cast(type, vec2->prop[1]);
			}

			return bv_variable_create_void();
		}

		/* floor() */
		bv_variable lib_glsl_floor(bv_program* prog, u8 count, bv_variable* args)
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
		bv_variable lib_glsl_texture(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* smp2D = bv_variable_get_object(args[0]);

					// sampler2D
					if (strcmp(smp2D->type->name, "sampler2D") == 0) {
						Texture* tex = (Texture*)smp2D->user_data;
						glm::vec2 uv = sd::AsVector<2, float>(args[1]); // TODO: vec2

						glm::vec4 sample = tex->Sample(uv.x, uv.y, 0.0f, 0.0f + bias);
						return create_vec4(prog, glm::vec4(sample)); // TODO: CreateVec4
					}
					/* else if samplerCube, isampler2D ... */
				}
			}

			return bv_variable_create_float(0.0f); // floor() must have 1 argument!
		}


		/* helper functions to create vector definitions */
		bv_object_info* add_vec(bv_library* lib, const char* name, u8 comp, u8 logNot = 0)
		{
			bv_object_info* vec = bv_object_info_create(name);

			if (comp >= 1) bv_object_info_add_property(vec, "x");
			if (comp >= 2) bv_object_info_add_property(vec, "y");
			if (comp >= 3) bv_object_info_add_property(vec, "z");
			if (comp >= 4) bv_object_info_add_property(vec, "w");

			bv_object_info_add_ext_method(vec, name, lib_glsl_vec_constructor);

			bv_object_info_add_ext_method(vec, "==", lib_glsl_vec_operator_equal);
			bv_object_info_add_ext_method(vec, "+", lib_glsl_vec_operator_add);
			bv_object_info_add_ext_method(vec, "-", lib_glsl_vec_operator_minus);
			bv_object_info_add_ext_method(vec, ">", lib_glsl_vec_operator_greater);
			bv_object_info_add_ext_method(vec, "/", lib_glsl_vec_operator_divide);
			bv_object_info_add_ext_method(vec, "*", lib_glsl_vec_operator_multiply);
			bv_object_info_add_ext_method(vec, "++", lib_glsl_vec_operator_increment);
			bv_object_info_add_ext_method(vec, "--", lib_glsl_vec_operator_decrement);
			if (logNot) bv_object_info_add_ext_method(vec, "!", lib_glsl_vec_operator_logical_not);

			bv_library_add_object_info(lib, vec);

			return vec;
		}

		bv_library* GLSL()
		{
			bv_library* lib = bv_library_create();

			// vector types
			add_vec(lib, "vec4", 4);
			add_vec(lib, "ivec4", 4);
			add_vec(lib, "bvec4", 4, 1);
			add_vec(lib, "dvec4", 4);
			add_vec(lib, "uvec4", 4);
			add_vec(lib, "vec3", 3);
			add_vec(lib, "ivec3", 3);
			add_vec(lib, "bvec3", 3, 1);
			add_vec(lib, "dvec3", 3);
			add_vec(lib, "uvec3", 3);
			add_vec(lib, "vec2", 2);
			add_vec(lib, "ivec2", 2);
			add_vec(lib, "bvec2", 2, 1);
			add_vec(lib, "dvec2", 2);
			add_vec(lib, "uvec2", 2);

			// sampler2D
			bv_object_info* sampler2D = bv_object_info_create("sampler2D");
			bv_library_add_object_info(lib, sampler2D);

			// floor()
			bv_library_add_function(lib, "floor", lib_glsl_floor);

			// texture()
			bv_library_add_function(lib, "texture", lib_glsl_texture);

			return lib;
		}
	}
}