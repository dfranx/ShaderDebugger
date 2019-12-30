#include <ShaderDebugger/GLSLLibrary.h>
#include <ShaderDebugger/Texture.h>
#include <ShaderDebugger/Matrix.h>
#include <ShaderDebugger/Utils.h>
#include <FastNoise/FastNoise.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

namespace sd
{
	namespace Library
	{
		/*
			TODO: 
				- dFdx, dFdy, dFdxCoarse, dFdyCoarse, dFdxFine, dFdyFine
				- fwidth, fwidthCoarse, fwidthFine
				- reimplement noise functions so that they follow the "rules"
		*/

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

		bv_variable create_mat(bv_program* prog, const char* name, sd::Matrix* mat)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, name));
			bv_object* retObj = bv_variable_get_object(ret);

			retObj->user_data = (void*)mat;

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
		glm::vec4 multiply_mat_vec(const sd::Matrix& mat, bv_object* vec, u8 transposed = 0)
		{
			glm::vec4 ret(0.0f);
			for (u16 i = 0; i < mat.Rows; i++)
				for (u16 j = 0; j < mat.Columns; j++) {
					float scalar = bv_variable_get_float(bv_variable_cast(bv_type_float, vec->prop[j]));
					if (transposed)
						ret[i] += mat.Data[i][j] * scalar;
					else
						ret[i] += mat.Data[j][i] * scalar;
				}

			return ret;
		}
		sd::Matrix* copy_mat_data(sd::Matrix* mat)
		{
			sd::Matrix* ret = new sd::Matrix();
			ret->Columns = mat->Columns;
			ret->Rows = mat->Rows;
			ret->Type = mat->Type;
			ret->Data = mat->Data;
			return ret;
		}

		/* swizzle */
		bv_variable GLSLswizzle(bv_program* prog, bv_object* obj, const bv_string field)
		{
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
				bv_object_set_property(vec, "w", bv_variable_create_pointer(bv_object_get_property(obj, name[3])));
				return ret;
			}
		}

		/* noise */
		static FastNoise lib_glsl_noiseGenerator; // I know... this takes least time, i'll have to reimplement noise functions anyway
		float noise1(glm::vec4 pos)
		{
			lib_glsl_noiseGenerator.SetNoiseType(FastNoise::NoiseType::WhiteNoise);
			lib_glsl_noiseGenerator.SetSeed(40000);
			return lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
		}
		glm::vec2 noise2(glm::vec4 pos)
		{
			glm::vec2 ret(0.0f);

			lib_glsl_noiseGenerator.SetNoiseType(FastNoise::NoiseType::WhiteNoise);
			lib_glsl_noiseGenerator.SetSeed(40000);
			ret.x = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(41000);
			ret.y = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);

			return ret;
		}
		glm::vec3 noise3(glm::vec4 pos)
		{
			glm::vec3 ret(0.0f);

			lib_glsl_noiseGenerator.SetNoiseType(FastNoise::NoiseType::WhiteNoise);
			lib_glsl_noiseGenerator.SetSeed(40000);
			ret.x = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(41000);
			ret.y = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(42000);
			ret.z = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);

			return ret;
		}
		glm::vec4 noise4(glm::vec4 pos)
		{
			glm::vec4 ret(0.0f);

			lib_glsl_noiseGenerator.SetNoiseType(FastNoise::NoiseType::WhiteNoise);
			lib_glsl_noiseGenerator.SetSeed(40000);
			ret.x = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(41000);
			ret.y = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(42000);
			ret.z = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(43000);
			ret.w = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);

			return ret;
		}

		/* vectors and operators */
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
			bv_variable ret = bv_variable_create_void();

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
			bv_variable ret = bv_variable_create_void();

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
			bv_variable ret = bv_variable_create_void();

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
			bv_variable ret = bv_variable_create_void();

			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// vec * vec
					if (vec->type->props.name_count) {
						bv_type mtype = merge_type(me->prop[0].type, vec->prop[0].type);

						ret = create_vec(prog, mtype, me->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < me->type->props.name_count; i++) {
							bv_variable_deinitialize(&retObj->prop[i]);
							retObj->prop[i] = bv_variable_cast(mtype, bv_variable_op_multiply(prog, me->prop[i], vec->prop[i]));
						}
					}
					// vec * mat
					else {
						sd::Matrix* matData = (sd::Matrix*)vec->user_data;
						glm::vec4 retVec = multiply_mat_vec(*matData, me, 1);
						ret = create_vec(prog, matData->Type, me->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);
						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_cast(matData->Type, bv_variable_create_float(retVec[i]));
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
		bv_variable lib_glsl_vec_operator_array_get(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();

			if (count == 1) {
				int index = 0;

				if (bv_type_is_integer(args[0].type))
					index = bv_variable_get_int(args[0]);
				else if (args[0].type == bv_type_float)
					index = bv_variable_get_float(args[0]);

				// TODO: how should we handle array index out of bounds?
				if (index >= me->type->props.name_count)
					index = me->type->props.name_count - 1;
				if (index < 0)
					index = 0;

				ret = bv_variable_copy(me->prop[index]);
			}

			return ret;
		}
		bv_variable lib_glsl_vec_operator_array_set(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			u8 indCount = 0;
			if (count > 1)
				indCount = bv_variable_get_int(args[0]);

			if (indCount == 1 && count > 2) {
				int index = 0;

				if (bv_type_is_integer(args[1].type))
					index = bv_variable_get_int(args[1]);
				else if (args[1].type == bv_type_float)
					index = bv_variable_get_float(args[1]);

				// TODO: how should we handle array index out of bounds?
				if (index >= me->type->props.name_count)
					index = me->type->props.name_count - 1;
				if (index < 0)
					index = 0;

				me->prop[index] = bv_variable_cast(me->prop[index].type, args[2]);
			}

			return bv_variable_create_void();
		}

		/* matrices and operators */
		bv_variable lib_glsl_mat_constructor(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			sd::Matrix* data = new sd::Matrix();
			sd::GetMatrixSizeFromName(me->type->name, &data->Columns, &data->Rows);
			data->Type = sd::GetMatrixTypeFromName(me->type->name);
			data->Data = glm::mat4(0.0f);

			bv_variable ret = bv_variable_create_void();
			me->user_data = (void*)data;

			if (count == 1) {
				// mat(mat)
				if (args[0].type == bv_type_object) {
					bv_object* mat = bv_variable_get_object(args[0]);
					sd::Matrix* copyData = (sd::Matrix*)mat->user_data;
					
					for (int x = 0; x < copyData->Columns && x < data->Columns; x++)
						for (int y = 0; y < copyData->Rows && y < data->Rows; y++)
							data->Data[x][y] = copyData->Data[x][y];
				}
				// mat(scalar)
				else {
					float scalar = bv_variable_get_float(bv_variable_cast(data->Type, args[0]));
					data->Data = glm::mat4(scalar);
				}

			}
			else if (count == data->Columns) // mat(col0, col1, ...)
			{
				for (u8 x = 0; x < data->Columns; x++) {
					bv_object* vecCol = bv_variable_get_object(args[x]);
					for (u8 y = 0; y < data->Rows && y < vecCol->type->props.name_count; y++)
						data->Data[x][y] = bv_variable_get_float(bv_variable_cast(data->Type, vecCol->prop[y]));
				}
			}
			else if (count == data->Columns * data->Rows) // mat(scalar, scalar, ...)
			{
				for (u8 x = 0; x < data->Columns; x++)
					for (u8 y = 0; y < data->Rows; y++) {
						float scalar = bv_variable_get_float(bv_variable_cast(data->Type, args[x*data->Columns + y]));
						data->Data[x][y] = scalar;
					}
			}

			return ret;
		}
		void lib_glsl_mat_destructor(bv_object* me)
		{
			if (me->user_data != 0)
				delete (sd::Matrix*)me->user_data;

			me->user_data = 0;
		}
		void* lib_glsl_mat_copy(void* udata)
		{
			if (udata != 0)
				return (void*)copy_mat_data((sd::Matrix*)udata);

			return 0;
		}
		bv_variable lib_glsl_mat_operator_multiply(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			sd::Matrix* myData = (sd::Matrix*)me->user_data;
			bv_variable ret = bv_variable_create_void();
			if (count == 1)
			{
				if (args[0].type == bv_type_object)
				{
					bv_object* obj = bv_variable_get_object(args[0]);

					// mat * mat
					if (obj->type->props.name_count == 0) {
						sd::Matrix* retMat = copy_mat_data(myData);
						retMat->Data = retMat->Data * ((sd::Matrix*)obj->user_data)->Data; // elements outside of the matrix should be 0.0f, so we don't care if we just multiply two 4x4 mats

						ret = create_mat(prog, me->type->name, retMat);
					}

					// mat * vec
					else
					{
						glm::vec4 retVec = multiply_mat_vec(*myData, obj);
						ret = create_vec(prog, myData->Type, obj->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);
						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_cast(myData->Type, bv_variable_create_float(retVec[i]));
					}
				}
			}
			return ret;
		}
		bv_variable lib_glsl_mat_operator_add(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			sd::Matrix* myData = (sd::Matrix*)me->user_data;
			bv_variable ret = bv_variable_create_void();
			if (count == 1)
			{
				if (args[0].type == bv_type_object)
				{
					bv_object* obj = bv_variable_get_object(args[0]);

					// mat + mat
					if (obj->type->props.name_count == 0) {
						sd::Matrix* retMat = copy_mat_data(myData);
						retMat->Data = retMat->Data + ((sd::Matrix*)obj->user_data)->Data;

						ret = create_mat(prog, me->type->name, retMat);
					}
				}
			}
			return ret;
		}
		bv_variable lib_glsl_mat_operator_equal(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			sd::Matrix* myData = (sd::Matrix*)me->user_data;
			bv_variable ret = bv_variable_create_uchar(0);
			if (count == 1)
			{
				if (args[0].type == bv_type_object)
				{
					bv_object* obj = bv_variable_get_object(args[0]);

					// mat == mat
					if (obj->type->props.name_count == 0) {
						u8 res = (myData->Data == ((sd::Matrix*)obj->user_data)->Data);
						bv_variable_set_uchar(&ret, res);
					}
				}
			}
			return ret;
		}
		bv_variable lib_glsl_mat_operator_increment(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			if (count == 0)
				((sd::Matrix*)me->user_data)->Data++;

			return bv_variable_create_void();
		}
		bv_variable lib_glsl_mat_operator_decrement(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			if (count == 0)
				((sd::Matrix*)me->user_data)->Data--;

			return bv_variable_create_void();
		}
		bv_variable lib_glsl_mat_operator_array_get(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();
			sd::Matrix* myData = (sd::Matrix*)me->user_data;

			if (count == 1) {
				int index = 0;

				if (bv_type_is_integer(args[0].type))
					index = bv_variable_get_int(args[0]);
				else if (args[0].type == bv_type_float)
					index = bv_variable_get_float(args[0]);

				// TODO: how should we handle array index out of bounds?
				if (index >= me->type->props.name_count)
					index = me->type->props.name_count - 1;
				if (index < 0)
					index = 0;

				ret = create_vec(prog, myData->Type, myData->Rows);

				bv_object* retObj = bv_variable_get_object(ret);
				for (u16 i = 0; i < retObj->type->props.name_count; i++)
					retObj->prop[i] = bv_variable_cast(myData->Type, bv_variable_create_float(myData->Data[index][i]));
			}

			return ret;
		}
		bv_variable lib_glsl_mat_operator_array_set(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			u8 argCount = count;
			u8 indCount = 0;
			if (argCount > 1)
				indCount = bv_variable_get_int(args[0]);

			sd::Matrix* myData = (sd::Matrix*)me->user_data;

			// set vector
			if (indCount == 1 && argCount > 2) {
				int col = 0;

				if (bv_type_is_integer(args[1].type))
					col = bv_variable_get_int(args[1]);
				else if (args[1].type == bv_type_float)
					col = bv_variable_get_float(args[1]);

				if (col >= myData->Columns)
					col = myData->Columns - 1;
				if (col < 0)
					col = 0;

				sd::Matrix* myData = (sd::Matrix*)me->user_data;

				bv_object* retObj = bv_variable_get_object(args[2]);
				for (u16 i = 0; i < retObj->type->props.name_count; i++)
					myData->Data[col][i] = bv_variable_get_float(bv_variable_cast(bv_type_float, retObj->prop[i]));
			}
			// set float
			else if (indCount == 2 && argCount > 3) {
				// column
				int col = 0;

				if (bv_type_is_integer(args[1].type))
					col = bv_variable_get_int(args[1]);
				else if (args[1].type == bv_type_float)
					col = bv_variable_get_float(args[1]);

				if (col >= myData->Columns)
					col = myData->Columns - 1;
				if (col < 0)
					col = 0;

				// row
				int row = 0;

				if (bv_type_is_integer(args[2].type))
					row = bv_variable_get_int(args[2]);
				else if (args[1].type == bv_type_float)
					row = bv_variable_get_float(args[2]);

				if (row >= myData->Rows)
					row = myData->Rows - 1;
				if (row < 0)
					row = 0;

				float scalar = bv_variable_get_float(bv_variable_cast(bv_type_float, args[3]));
				myData->Data[col][row] = scalar;
			}

			return bv_variable_create_void();
		}
		
		/* trigonometry */
		bv_variable lib_glsl_acos(bv_program* prog, u8 count, bv_variable* args)
		{
			/* acos(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // acos(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::acos(sd::AsVector<4, float>(args[0])); // acos takes only float vectors

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // acos(scalar)
					return bv_variable_create_float(glm::acos(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f); // acos() must have 1 argument!
		}
		bv_variable lib_glsl_acosh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* acosh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // acosh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::acosh(sd::AsVector<4, float>(args[0])); // acosh takes only float vectors

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // acosh(scalar)
					return bv_variable_create_float(glm::acosh(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f); // floor() must have 1 argument!
		}
		bv_variable lib_glsl_asin(bv_program* prog, u8 count, bv_variable* args)
		{
			/* asin(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // asin(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::asin(sd::AsVector<4, float>(args[0])); // asin takes only float vectors

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else
					return bv_variable_create_float(glm::asin(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f); // asin() must have 1 argument!
		}
		bv_variable lib_glsl_asinh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* asinh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // asinh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::asinh(sd::AsVector<4, float>(args[0])); // asinh takes only float vectors

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // asinh(scalar)
					return bv_variable_create_float(glm::asinh(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_atan(bv_program* prog, u8 count, bv_variable* args)
		{
			/* atan(genType y_over_x) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // atan(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::atan(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else
					return bv_variable_create_float(glm::atan(bv_variable_get_float(args[0])));
			}
			/* atan(genType y, genType x) */
			else if (count == 2) {
				if (args[0].type == bv_type_object) { // atan(vec3), ...
					bv_object* y = bv_variable_get_object(args[0]);
					glm::vec4 yData = sd::AsVector<4, float>(args[0]);

					bv_object* x = bv_variable_get_object(args[1]);
					glm::vec4 xData = sd::AsVector<4, float>(args[1]);

					bv_variable ret = create_vec(prog, bv_type_float, y->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(glm::atan(yData[i], xData[i]));

					return ret;
				}
				else
					return bv_variable_create_float(
						glm::atan(
							bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 
							bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]))
						)
					);
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_atanh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* atanh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // atanh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::atanh(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // atanh(scalar)
					return bv_variable_create_float(glm::atanh(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_cos(bv_program* prog, u8 count, bv_variable* args)
		{
			/* cos(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // cos(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::cos(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // cos(scalar)
					return bv_variable_create_float(glm::cos(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_cosh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* cosh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // cosh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::cosh(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // cosh(scalar)
					return bv_variable_create_float(glm::cosh(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_sin(bv_program* prog, u8 count, bv_variable* args)
		{
			/* sin(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // sin(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::sin(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else
					return bv_variable_create_float(glm::sin(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_sinh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* sinh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // sinh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::sinh(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // sinh(scalar)
					return bv_variable_create_float(glm::sinh(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_tan(bv_program* prog, u8 count, bv_variable* args)
		{
			/* tan(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // tan(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::tan(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // tan(scalar)
					return bv_variable_create_float(glm::tan(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_tanh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* tanh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // tanh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::tanh(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // tanh(scalar)
					return bv_variable_create_float(glm::tanh(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_degrees(bv_program* prog, u8 count, bv_variable* args)
		{
			/* degrees(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // degrees(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::degrees(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // degrees(scalar)
					return bv_variable_create_float(glm::degrees(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_radians(bv_program* prog, u8 count, bv_variable* args)
		{
			/* radians(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // radians(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::radians(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // radians(scalar)
					return bv_variable_create_float(glm::radians(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}

		/* mathematics */
		bv_variable lib_glsl_abs(bv_program* prog, u8 count, bv_variable* args)
		{
			/* abs(genType), abs(genIType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// abs(vec)
					if (vec->prop[0].type == bv_type_float) {
						glm::vec4 vecData = glm::abs(sd::AsVector<4, float>(args[0]));

						bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(vecData[i]);

						return ret;
					}
					// abs(ivec)
					else if (vec->prop[0].type == bv_type_int) {
						glm::ivec4 vecData = glm::abs(sd::AsVector<4, int>(args[0])); // acos takes only float vectors

						bv_variable ret = create_vec(prog, bv_type_int, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_int(vecData[i]);

						return ret;
					}
				}
				else if (args[0].type == bv_type_float)
					return bv_variable_create_float(glm::abs(bv_variable_get_float(args[0])));
				else
					return bv_variable_create_int(glm::abs(bv_variable_get_int(args[0])));
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_ceil(bv_program* prog, u8 count, bv_variable* args)
		{
			/* ceil(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::ceil(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // ceil(scalar)
					return bv_variable_create_float(glm::ceil(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_clamp(bv_program* prog, u8 count, bv_variable* args)
		{
			/* clamp(genType, genType, genType), clamp(genType, float, float), also for genIType and genUType */
			if (count == 3) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// using: genType, float, genDType, double
					if (vec->prop[0].type == bv_type_float)
					{
						glm::vec4 minVal(0.0f), maxVal(0.0f);
						
						// get minBound
						if (args[1].type == bv_type_object)
							minVal = sd::AsVector<4, float>(args[1]);
						else
							minVal = glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[1])));

						// get maxBound
						if (args[2].type == bv_type_object)
							maxVal = sd::AsVector<4, float>(args[2]);
						else
							maxVal = glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[2])));

						glm::vec4 vecData = glm::clamp(sd::AsVector<4, float>(args[0]), minVal, maxVal);

						bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(vecData[i]);

						return ret;
					}
					// using: genUType, uint
					else if (vec->prop[0].type == bv_type_uint)
					{
						glm::uvec4 minVal(0u), maxVal(0u);

						// get minBound
						if (args[1].type == bv_type_object)
							minVal = sd::AsVector<4, unsigned int>(args[1]);
						else
							minVal = glm::uvec4(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[1])));

						// get maxBound
						if (args[2].type == bv_type_object)
							maxVal = sd::AsVector<4, unsigned int>(args[2]);
						else
							maxVal = glm::uvec4(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[2])));

						glm::uvec4 vecData = glm::clamp(sd::AsVector<4, unsigned int>(args[0]), minVal, maxVal);

						bv_variable ret = create_vec(prog, bv_type_uint, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_uint(vecData[i]);

						return ret;
					}
					// using: genIType, int
					else if (vec->prop[0].type == bv_type_int)
					{
						glm::ivec4 minVal(0u), maxVal(0u);

						// get minBound
						if (args[1].type == bv_type_object)
							minVal = sd::AsVector<4, int>(args[1]);
						else
							minVal = glm::ivec4(bv_variable_get_int(bv_variable_cast(bv_type_int, args[1])));

						// get maxBound
						if (args[2].type == bv_type_object)
							maxVal = sd::AsVector<4, int>(args[2]);
						else
							maxVal = glm::ivec4(bv_variable_get_int(bv_variable_cast(bv_type_int, args[2])));

						glm::ivec4 vecData = glm::clamp(sd::AsVector<4, int>(args[0]), minVal, maxVal);

						bv_variable ret = create_vec(prog, bv_type_int, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_int(vecData[i]);

						return ret;
					}

				}

				// clamp(float, float, float)
				else if (args[0].type == bv_type_float) {
					float val = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					float minVal = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));
					float maxVal = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

					return bv_variable_create_float(glm::clamp(val, minVal, maxVal));
				}

				// clamp(uint, uint, uint)
				else if (args[0].type == bv_type_uint) {
					unsigned int val = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]));
					unsigned int minVal = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[1]));
					unsigned int maxVal = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[2]));

					return bv_variable_create_uint(glm::clamp(val, minVal, maxVal));
				}

				// clamp(int, int, int)
				else if (args[0].type == bv_type_int) {
					int val = bv_variable_get_int(bv_variable_cast(bv_type_int, args[0]));
					int minVal = bv_variable_get_int(bv_variable_cast(bv_type_int, args[1]));
					int maxVal = bv_variable_get_int(bv_variable_cast(bv_type_int, args[2]));

					return bv_variable_create_int(glm::clamp(val, minVal, maxVal));
				}

			}

			return bv_variable_create_float(0.0f); // floor() must have 1 argument!
		}
		bv_variable lib_glsl_dFdx(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			
			// IMPLEMENTATION IDEA: prog->user_data is pointer to sd::ShaderDebugger, maybe call dbg->Execute()
			// 2x2 (the block of fragments) and just shift gl_FragCoord.xy since we know
			// that the user is running GLSL code. 
			// OR: make ShaderDebugger run the shader for 3 other values beforehand
			
			/* dFdx(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdx(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return create_vec(prog, bv_type_float, vec->type->props.name_count);
				} else {} // dFdx(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_dFdy(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			/* dFdy(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdy(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdy(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_dFdxCoarse(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			/* dFdxCoarse(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdxCoarse(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdxCoarse(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_dFdyCoarse(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			/* dFdyCoarse(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdyCoarse(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdyCoarse(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_dFdxFine(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			/* dFdxFine(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdxFine(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdxFine(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_dFdyFine(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			/* dFdyFine(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdyFine(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdyFine(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_exp(bv_program* prog, u8 count, bv_variable* args)
		{
			/* exp(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // exp(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::exp(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // exp(scalar)
					return bv_variable_create_float(glm::exp(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_exp2(bv_program* prog, u8 count, bv_variable* args)
		{
			/* exp2(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // exp2(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::exp2(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // exp2(scalar)
					return bv_variable_create_float(glm::exp2(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_floor(bv_program* prog, u8 count, bv_variable* args)
		{
			/* floor(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::floor(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // floor(scalar)
					return bv_variable_create_float(glm::floor(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_fma(bv_program* prog, u8 count, bv_variable* args)
		{
			// a * b + c
			/* fma(genType, genType, genType), fma(genDType, genDType, genDType)  */
			if (count == 3) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::vec4 a = sd::AsVector<4, float>(args[0]);
					glm::vec4 b = sd::AsVector<4, float>(args[1]);
					glm::vec4 c = sd::AsVector<4, float>(args[2]);

					glm::vec4 vecData = glm::fma(a,b,c);

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;

				}

				// fma(float, float, float)
				else {
					float a = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					float b = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));
					float c = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

					return bv_variable_create_float(glm::fma(a,b,c));
				}
			}

			return bv_variable_create_float(0.0f); // floor() must have 1 argument!
		}
		bv_variable lib_glsl_fract(bv_program* prog, u8 count, bv_variable* args)
		{
			/* fract(genType), fract(genDType)  */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::vec4 vecData = glm::fract(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;

				}
				// fract(float)
				else {
					float scalarData = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					return bv_variable_create_float(glm::fract(scalarData));
				}
			}

			return bv_variable_create_float(0.0f); // floor() must have 1 argument!
		}
		bv_variable lib_glsl_fwidth(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO: requires dFdx/y */
			/* fwidth(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // fwidth(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // fwidth(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_fwidthCoarse(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO: requires dFdx/yCoarse */
			/* fwidthCoarse(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // fwidthCoarse(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // fwidthCoarse(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_fwidthFine(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO: requires dFdx/yFine */
			/* fwidthFine(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // fwidthFine(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // fwidthFine(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_inversesqrt(bv_program* prog, u8 count, bv_variable* args)
		{
			/* inversesqrt(genType), inversesqrt(genDType)  */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::vec4 vecData = glm::inversesqrt(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;

				}
				// inversesqrt(float)
				else {
					float scalarData = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					return bv_variable_create_float(glm::inversesqrt(scalarData));
				}
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_isinf(bv_program* prog, u8 count, bv_variable* args)
		{
			/* isinf(genType), isinf(genDType)  */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::bvec4 vecData = glm::isinf(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_uchar, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uchar(vecData[i]);

					return ret;

				}
				// isinf(float)
				else {
					float scalarData = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					return bv_variable_create_uchar(glm::isinf(scalarData));
				}
			}

			return bv_variable_create_uchar(0);
		}
		bv_variable lib_glsl_isnan(bv_program* prog, u8 count, bv_variable* args)
		{
			/* isnan(genType), isnan(genDType)  */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::bvec4 vecData = glm::isnan(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_uchar, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uchar(vecData[i]);

					return ret;

				}
				// isnan(float)
				else {
					float scalarData = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					return bv_variable_create_uchar(glm::isnan(scalarData));
				}
			}

			return bv_variable_create_uchar(0);
		}
		bv_variable lib_glsl_log(bv_program* prog, u8 count, bv_variable* args)
		{
			/* log(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // log(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::log(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // log(scalar)
					return bv_variable_create_float(glm::log(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_log2(bv_program* prog, u8 count, bv_variable* args)
		{
			/* log2(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // exp2(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::log2(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // log2(scalar)
					return bv_variable_create_float(glm::log2(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_max(bv_program* prog, u8 count, bv_variable* args)
		{
			/* max(genType, genType), max(genType, float), also for genIType and genUType */
			if (count == 2) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// using: genType, float, genDType, double
					if (vec->prop[0].type == bv_type_float)
					{
						glm::vec4 x = sd::AsVector<4, float>(args[0]);
						glm::vec4 y(0.0f);

						// get y
						if (args[1].type == bv_type_object)
							y = sd::AsVector<4, float>(args[1]);
						else
							y = glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[1])));

						glm::vec4 vecData = glm::max(x, y);

						bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(vecData[i]);

						return ret;
					}
					// using: genUType, uint
					else if (vec->prop[0].type == bv_type_uint)
					{
						glm::uvec4 x = sd::AsVector<4, unsigned int>(args[0]);
						glm::uvec4 y(0u);

						// get y
						if (args[1].type == bv_type_object)
							y = sd::AsVector<4, unsigned int>(args[1]);
						else
							y = glm::uvec4(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[1])));

						glm::uvec4 vecData = glm::max(x, y);

						bv_variable ret = create_vec(prog, bv_type_uint, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_uint(vecData[i]);

						return ret;
					}
					// using: genIType, int
					else if (vec->prop[0].type == bv_type_int)
					{
						glm::ivec4 x = sd::AsVector<4, int>(args[0]);
						glm::ivec4 y(0);

						// get y
						if (args[1].type == bv_type_object)
							y = sd::AsVector<4, int>(args[1]);
						else
							y = glm::ivec4(bv_variable_get_int(bv_variable_cast(bv_type_int, args[1])));

						glm::ivec4 vecData = glm::max(x, y);

						bv_variable ret = create_vec(prog, bv_type_int, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_int(vecData[i]);

						return ret;
					}

				}

				// max(float, float)
				else if (args[0].type == bv_type_float) {
					float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					float y = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));

					return bv_variable_create_float(glm::max(x, y));
				}

				// max(uint, uint)
				else if (args[0].type == bv_type_uint) {
					unsigned int x = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]));
					unsigned int y = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[1]));

					return bv_variable_create_uint(glm::max(x, y));
				}

				// max(int, int)
				else if (args[0].type == bv_type_int) {
					int x = bv_variable_get_int(bv_variable_cast(bv_type_int, args[0]));
					int y = bv_variable_get_int(bv_variable_cast(bv_type_int, args[1]));

					return bv_variable_create_int(glm::max(x, y));
				}

			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_min(bv_program* prog, u8 count, bv_variable* args)
		{
			/* min(genType, genType), min(genType, float), also for genIType and genUType */
			if (count == 2) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// using: genType, float, genDType, double
					if (vec->prop[0].type == bv_type_float)
					{
						glm::vec4 x = sd::AsVector<4, float>(args[0]);
						glm::vec4 y(0.0f);

						// get y
						if (args[1].type == bv_type_object)
							y = sd::AsVector<4, float>(args[1]);
						else
							y = glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[1])));

						glm::vec4 vecData = glm::min(x, y);

						bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(vecData[i]);

						return ret;
					}
					// using: genUType, uint
					else if (vec->prop[0].type == bv_type_uint)
					{
						glm::uvec4 x = sd::AsVector<4, unsigned int>(args[0]);
						glm::uvec4 y(0u);

						// get y
						if (args[1].type == bv_type_object)
							y = sd::AsVector<4, unsigned int>(args[1]);
						else
							y = glm::uvec4(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[1])));

						glm::uvec4 vecData = glm::min(x, y);

						bv_variable ret = create_vec(prog, bv_type_uint, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_uint(vecData[i]);

						return ret;
					}
					// using: genIType, int
					else if (vec->prop[0].type == bv_type_int)
					{
						glm::ivec4 x = sd::AsVector<4, int>(args[0]);
						glm::ivec4 y(0);

						// get y
						if (args[1].type == bv_type_object)
							y = sd::AsVector<4, int>(args[1]);
						else
							y = glm::ivec4(bv_variable_get_int(bv_variable_cast(bv_type_int, args[1])));

						glm::ivec4 vecData = glm::min(x, y);

						bv_variable ret = create_vec(prog, bv_type_int, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_int(vecData[i]);

						return ret;
					}

				}

				// min(float, float)
				else if (args[0].type == bv_type_float) {
					float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					float y = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));

					return bv_variable_create_float(glm::min(x, y));
				}

				// min(uint, uint)
				else if (args[0].type == bv_type_uint) {
					unsigned int x = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]));
					unsigned int y = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[1]));

					return bv_variable_create_uint(glm::min(x, y));
				}

				// min(int, int)
				else if (args[0].type == bv_type_int) {
					int x = bv_variable_get_int(bv_variable_cast(bv_type_int, args[0]));
					int y = bv_variable_get_int(bv_variable_cast(bv_type_int, args[1]));

					return bv_variable_create_int(glm::min(x, y));
				}

			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_mix(bv_program* prog, u8 count, bv_variable* args)
		{
			/* mix(genType, genType, genType), mix(genType, genType, float), mix(genI/U/B/Type, genI/U/BType, genBType) */
			if (count == 3) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// using: genType, float, genDType, double, genBType
					if (vec->prop[0].type == bv_type_float)
					{
						glm::vec4 x = sd::AsVector<4, float>(args[0]);
						glm::vec4 y = sd::AsVector<4, float>(args[1]);
						glm::vec4 a(0.0f);

						// get a
						if (args[2].type == bv_type_object)
							a = sd::AsVector<4, float>(args[2]);
						else
							a = glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[2])));

						glm::vec4 vecData = glm::mix(x, y, a);

						bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(vecData[i]);

						return ret;
					}
					// using: genUType, genBType
					else if (vec->prop[0].type == bv_type_uint)
					{
						glm::uvec4 x = sd::AsVector<4, unsigned int>(args[0]);
						glm::uvec4 y = sd::AsVector<4, unsigned int>(args[1]);
						glm::vec4 a = sd::AsVector<4, float>(args[2]);

						glm::uvec4 vecData = glm::mix(x, y, a);

						bv_variable ret = create_vec(prog, bv_type_uint, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_uint(vecData[i]);

						return ret;
					}
					// using: genIType, genBType
					else if (vec->prop[0].type == bv_type_int)
					{
						glm::ivec4 x = sd::AsVector<4, int>(args[0]);
						glm::ivec4 y = sd::AsVector<4, int>(args[1]);
						glm::vec4 a = sd::AsVector<4, float>(args[2]);

						glm::ivec4 vecData = glm::mix(x, y, a);

						bv_variable ret = create_vec(prog, bv_type_int, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_int(vecData[i]);

						return ret;
					}
					// using: genBType, genBType
					else if (vec->prop[0].type == bv_type_uchar)
					{
						glm::bvec4 x = sd::AsVector<4, bool>(args[0]);
						glm::bvec4 y = sd::AsVector<4, bool>(args[1]);
						glm::vec4 a = sd::AsVector<4, float>(args[2]);

						glm::bvec4 vecData = glm::mix(x, y, a);

						bv_variable ret = create_vec(prog, bv_type_uchar, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_uchar(vecData[i]);

						return ret;
					}

				}

				// mix(float, float, float/bool)
				else if (args[0].type == bv_type_float) {
					float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					float y = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));
					float a = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

					return bv_variable_create_float(glm::mix(x, y, a));
				}

				// mix(uint, uint, bool)
				else if (args[0].type == bv_type_uint) {
					unsigned int x = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]));
					unsigned int y = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[1]));
					float a = (float)bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[2]));

					return bv_variable_create_uint(glm::mix(x, y, a));
				}

				// mix(int, int, bool)
				else if (args[0].type == bv_type_int) {
					int x = bv_variable_get_int(bv_variable_cast(bv_type_int, args[0]));
					int y = bv_variable_get_int(bv_variable_cast(bv_type_int, args[1]));
					float a = (float)bv_variable_get_int(bv_variable_cast(bv_type_int, args[2]));

					return bv_variable_create_int(glm::mix(x, y, a));
				}

				// mix(bool, bool, bool)
				else if (args[0].type == bv_type_uchar) {
					bool x = bv_variable_get_uchar(bv_variable_cast(bv_type_uchar, args[0]));
					bool y = bv_variable_get_uchar(bv_variable_cast(bv_type_uchar, args[1]));
					float a = (float)bv_variable_get_uchar(bv_variable_cast(bv_type_uchar, args[2]));

					return bv_variable_create_uchar(glm::mix(x, y, a));
				}
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_mod(bv_program* prog, u8 count, bv_variable* args)
		{
			/* mod(genType, genType), mod(genType, float), also for genDType */
			if (count == 2) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// using: genType, float, genDType, double
					glm::vec4 x = sd::AsVector<4, float>(args[0]);
					glm::vec4 y(0.0f);

					// get y
					if (args[1].type == bv_type_object)
						y = sd::AsVector<4, float>(args[1]);
					else
						y = glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[1])));

					glm::vec4 vecData = glm::mod(x, y);

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}

				// mod(float, float)
				else if (args[0].type == bv_type_float) {
					float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					float y = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));

					return bv_variable_create_float(glm::mod(x, y));
				}
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_modf(bv_program* prog, u8 count, bv_variable* args)
		{
			/* modf(genType, out genType), modf(float, out float), also for genDType */
			if (count == 2) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// using: genType, float, genDType, double
					glm::vec4 x = sd::AsVector<4, float>(args[0]);
					glm::vec4 retData(0.0f), outValData(0.0f);

					retData = glm::modf(x, outValData);

					// param out value
					bv_variable* outPtr = bv_variable_get_pointer(args[1]);
					bv_object* outObj = bv_variable_get_object(*outPtr);
					for (u16 i = 0; i < outObj->type->props.name_count; i++)
						outObj->prop[i] = bv_variable_create_float(outValData[i]);

					// return value
					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);
					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(retData[i]);

					return ret;
				}

				// modf(float, float)
				else if (args[0].type == bv_type_float) {
					float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					float outVal = 0.0f;
					
					float retVal = glm::modf(x, outVal);

					bv_variable* outPtr = bv_variable_get_pointer(args[1]);
					bv_variable_set_float(outPtr, outVal);

					return bv_variable_create_float(retVal);
				}
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_noise1(bv_program* prog, u8 count, bv_variable* args)
		{
			/* noise1(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::ceil(sd::AsVector<4, float>(args[0]));

					return bv_variable_create_float(noise1(vecData));
				}
				else // noise1(scalar)
					return bv_variable_create_float(noise1(glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 0.0f, 0.0f, 0.0f)));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_noise2(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = create_vec2(prog);
			bv_object* retObj = bv_variable_get_object(ret);

			/* noise2(genType) */
			if (count == 1) {
				glm::vec2 noiseData(0.0f);

				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::ceil(sd::AsVector<4, float>(args[0]));

					noiseData = noise2(vecData);
				}
				else // noise2(scalar)
					noiseData = noise2(glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 0.0f, 0.0f, 0.0f));

				retObj->prop[0] = bv_variable_create_float(noiseData.x);
				retObj->prop[1] = bv_variable_create_float(noiseData.y);
			}

			return ret;
		}
		bv_variable lib_glsl_noise3(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = create_vec3(prog);
			bv_object* retObj = bv_variable_get_object(ret);

			/* noise2(genType) */
			if (count == 1) {
				glm::vec3 noiseData(0.0f);

				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::ceil(sd::AsVector<4, float>(args[0]));

					noiseData = noise3(vecData);
				}
				else // noise2(scalar)
					noiseData = noise3(glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 0.0f, 0.0f, 0.0f));

				retObj->prop[0] = bv_variable_create_float(noiseData.x);
				retObj->prop[1] = bv_variable_create_float(noiseData.y);
				retObj->prop[2] = bv_variable_create_float(noiseData.z);
			}

			return ret;
		}
		bv_variable lib_glsl_noise4(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = create_vec4(prog);
			bv_object* retObj = bv_variable_get_object(ret);

			/* noise2(genType) */
			if (count == 1) {
				glm::vec4 noiseData(0.0f);

				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::ceil(sd::AsVector<4, float>(args[0]));

					noiseData = noise4(vecData);
				}
				else // noise2(scalar)
					noiseData = noise4(glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 0.0f, 0.0f, 0.0f));

				retObj->prop[0] = bv_variable_create_float(noiseData.x);
				retObj->prop[1] = bv_variable_create_float(noiseData.y);
				retObj->prop[2] = bv_variable_create_float(noiseData.z);
				retObj->prop[3] = bv_variable_create_float(noiseData.w);
			}

			return ret;
		}
		bv_variable lib_glsl_pow(bv_program* prog, u8 count, bv_variable* args)
		{
			/* pow(genType y, genType x) */
			if (count == 2) {
				if (args[0].type == bv_type_object) {
					bv_object* x = bv_variable_get_object(args[0]);
					glm::vec4 xData = sd::AsVector<4, float>(args[0]);

					bv_object* y = bv_variable_get_object(args[1]);
					glm::vec4 yData = sd::AsVector<4, float>(args[1]);

					glm::vec4 resData = glm::pow(xData, yData);

					bv_variable ret = create_vec(prog, bv_type_float, y->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(resData[i]);

					return ret;
				}
				else
					return bv_variable_create_float(
						glm::pow(
							bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])),
							bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]))
						)
					);
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_round(bv_program* prog, u8 count, bv_variable* args)
		{
			/* round(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::round(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // cos(scalar)
					return bv_variable_create_float(glm::round(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_roundEven(bv_program* prog, u8 count, bv_variable* args)
		{
			/* round(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::roundEven(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // cos(scalar)
					return bv_variable_create_float(glm::roundEven(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_sign(bv_program* prog, u8 count, bv_variable* args)
		{
			/* sign(genType), sign(genIType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// sign(vec)
					if (vec->prop[0].type == bv_type_float) {
						glm::vec4 vecData = glm::sign(sd::AsVector<4, float>(args[0]));

						bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(vecData[i]);

						return ret;
					}
					// sign(ivec)
					else if (vec->prop[0].type == bv_type_int) {
						glm::ivec4 vecData = glm::sign(sd::AsVector<4, int>(args[0])); // acos takes only float vectors

						bv_variable ret = create_vec(prog, bv_type_int, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_int(vecData[i]);

						return ret;
					}
				}
				else if (args[0].type == bv_type_float)
					return bv_variable_create_float(glm::sign(bv_variable_get_float(args[0])));
				else
					return bv_variable_create_int(glm::sign(bv_variable_get_int(args[0])));
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_smoothstep(bv_program* prog, u8 count, bv_variable* args)
		{
			/* mix(genType, genType, genType), mix(float, float, genType) also for genDType */
			if (count == 3) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// using: genType, float, genDType, double, genBType
					glm::vec4 edge0 = sd::AsVector<4, float>(args[0]);
					glm::vec4 edge1 = sd::AsVector<4, float>(args[1]);
					glm::vec4 x(0.0f);

					// get a
					if (args[2].type == bv_type_object)
						x = sd::AsVector<4, float>(args[2]);
					else
						x = glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[2])));
					

					glm::vec4 vecData(0.0f);
					
					u8 vecSize = vec->type->props.name_count;

					if (vecSize == 2) {
						glm::vec2 res = glm::smoothstep(glm::vec2(edge0.x, edge0.y), glm::vec2(edge1.x, edge1.y), glm::vec2(x.x, x.y));
						vecData.x = res.x; vecData.y = res.y;
					}
					else if (vecSize == 3) {
						glm::vec3 res = glm::smoothstep(glm::vec3(edge0.x, edge0.y, edge0.z), glm::vec3(edge1.x, edge1.y, edge1.z), glm::vec3(x.x, x.y, x.z));
						vecData.x = res.x; vecData.y = res.y; vecData.z = res.z;
					}
					else if (vecSize == 4)
						vecData = glm::smoothstep(edge0, edge1, x);

					bv_variable ret = create_vec(prog, bv_type_float, vecSize);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}

				// smoothstep(float, float, genType)
				else {
					float edge0 = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					float edge1 = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));

					
					// get x
					if (args[2].type == bv_type_object) {
						glm::vec4 x = sd::AsVector<4, float>(args[2]);

						bv_object* vec = bv_variable_get_object(args[2]);

						glm::vec4 vecData(0.0f);

						u8 vecSize = vec->type->props.name_count;

						if (vecSize == 2) {
							glm::vec2 res = glm::smoothstep(edge0, edge1, glm::vec2(x.x, x.y));
							vecData.x = res.x; vecData.y = res.y;
						}
						else if (vecSize == 3) {
							glm::vec3 res = glm::smoothstep(edge0, edge1, glm::vec3(x.x, x.y, x.z));
							vecData.x = res.x; vecData.y = res.y; vecData.z = res.z;
						}
						else if (vecSize == 4)
							vecData = glm::smoothstep(edge0, edge1, x);


						bv_variable ret = create_vec(prog, bv_type_float, vecSize);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(vecData[i]);

						return ret;
					}
					else {
						float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));
						return bv_variable_create_float(glm::smoothstep(edge0, edge1, x));
					}
				}
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_sqrt(bv_program* prog, u8 count, bv_variable* args)
		{
			/* sqrt(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // sqrt(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::sqrt(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // sqrt(scalar)
					return bv_variable_create_float(glm::sqrt(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_step(bv_program* prog, u8 count, bv_variable* args)
		{
			/* step(genType, genType), step(float, genType) */
			if (count == 2) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::vec4 edge = sd::AsVector<4, float>(args[0]);
					glm::vec4 x(0.0f);

					// get x
					if (args[1].type == bv_type_object)
						x = sd::AsVector<4, float>(args[1]);
					else
						x = glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[1])));

					glm::vec4 vecData = glm::step(edge, x);

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}

				// step(float, float)
				else if (args[0].type == bv_type_float) {
					glm::vec4 edge(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])));

					// get x
					if (args[1].type == bv_type_object) {
						glm::vec4 x = sd::AsVector<4, float>(args[1]);
						bv_object* vec = bv_variable_get_object(args[1]);

						glm::vec4 retData = glm::step(edge, x);

						bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(retData[i]);

						return ret;
					} else
						return bv_variable_create_float(
									glm::step(edge.x, bv_variable_get_float(bv_variable_cast(bv_type_float, args[1])))
								);
				}

			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_trunc(bv_program* prog, u8 count, bv_variable* args)
		{
			/* trunc(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // trunc(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::trunc(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // trunc(scalar)
					return bv_variable_create_float(glm::trunc(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}

		/* texture() */
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


		/* helper functions to create vector & matrix definitions */
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
			bv_object_info_add_ext_method(vec, "[]", lib_glsl_vec_operator_array_get);
			bv_object_info_add_ext_method(vec, "[]=", lib_glsl_vec_operator_array_set);
			if (logNot) bv_object_info_add_ext_method(vec, "!", lib_glsl_vec_operator_logical_not);

			bv_library_add_object_info(lib, vec);

			return vec;
		}
		bv_object_info* add_mat(bv_library* lib, const char* name)
		{
			bv_object_info* mat = bv_object_info_create(name);
			mat->on_delete = lib_glsl_mat_destructor;
			mat->on_copy = lib_glsl_mat_copy;

			bv_object_info_add_ext_method(mat, name, lib_glsl_mat_constructor);

			bv_object_info_add_ext_method(mat, "*", lib_glsl_mat_operator_multiply);
			bv_object_info_add_ext_method(mat, "+", lib_glsl_mat_operator_add);
			bv_object_info_add_ext_method(mat, "++", lib_glsl_mat_operator_increment);
			bv_object_info_add_ext_method(mat, "--", lib_glsl_mat_operator_decrement);
			bv_object_info_add_ext_method(mat, "==", lib_glsl_mat_operator_equal);
			bv_object_info_add_ext_method(mat, "[]", lib_glsl_mat_operator_array_get);
			bv_object_info_add_ext_method(mat, "[]=", lib_glsl_mat_operator_array_set);

			bv_library_add_object_info(lib, mat);

			return mat;
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

			// matrix types
			add_mat(lib, "mat2");
			add_mat(lib, "mat3");
			add_mat(lib, "mat4");
			add_mat(lib, "mat2x2");
			add_mat(lib, "mat2x3");
			add_mat(lib, "mat2x4");
			add_mat(lib, "mat3x2");
			add_mat(lib, "mat3x3");
			add_mat(lib, "mat3x4");
			add_mat(lib, "mat4x2");
			add_mat(lib, "mat4x3");
			add_mat(lib, "mat4x4");
			add_mat(lib, "dmat2");
			add_mat(lib, "dmat3");
			add_mat(lib, "dmat4");
			add_mat(lib, "dmat2x2");
			add_mat(lib, "dmat2x3");
			add_mat(lib, "dmat2x4");
			add_mat(lib, "dmat3x2");
			add_mat(lib, "dmat3x3");
			add_mat(lib, "dmat3x4");
			add_mat(lib, "dmat4x2");
			add_mat(lib, "dmat4x3");
			add_mat(lib, "dmat4x4");

			// sampler2D
			bv_object_info* sampler2D = bv_object_info_create("sampler2D");
			bv_library_add_object_info(lib, sampler2D);

			// trigonometry
			bv_library_add_function(lib, "acos", lib_glsl_acos);
			bv_library_add_function(lib, "acosh", lib_glsl_acosh);
			bv_library_add_function(lib, "asin", lib_glsl_asin);
			bv_library_add_function(lib, "asinh", lib_glsl_asinh);
			bv_library_add_function(lib, "atan", lib_glsl_atan);
			bv_library_add_function(lib, "atanh", lib_glsl_atanh);
			bv_library_add_function(lib, "acos", lib_glsl_cos);
			bv_library_add_function(lib, "acosh", lib_glsl_cosh);
			bv_library_add_function(lib, "asin", lib_glsl_sin);
			bv_library_add_function(lib, "asinh", lib_glsl_sinh);
			bv_library_add_function(lib, "atan", lib_glsl_tan);
			bv_library_add_function(lib, "atanh", lib_glsl_tanh);
			bv_library_add_function(lib, "degrees", lib_glsl_degrees);
			bv_library_add_function(lib, "radians", lib_glsl_radians);

			// mathematics
			bv_library_add_function(lib, "abs", lib_glsl_abs);
			bv_library_add_function(lib, "ceil", lib_glsl_ceil);
			bv_library_add_function(lib, "clamp", lib_glsl_clamp);
			bv_library_add_function(lib, "dFdx", lib_glsl_dFdx);
			bv_library_add_function(lib, "dFdy", lib_glsl_dFdy);
			bv_library_add_function(lib, "dFdxCoarse", lib_glsl_dFdxCoarse);
			bv_library_add_function(lib, "dFdyCoarse", lib_glsl_dFdyCoarse);
			bv_library_add_function(lib, "dFdxFine", lib_glsl_dFdxFine);
			bv_library_add_function(lib, "dFdyFine", lib_glsl_dFdyFine);
			bv_library_add_function(lib, "exp", lib_glsl_exp);
			bv_library_add_function(lib, "exp2", lib_glsl_exp2);
			bv_library_add_function(lib, "floor", lib_glsl_floor);
			bv_library_add_function(lib, "fma", lib_glsl_fma);
			bv_library_add_function(lib, "fract", lib_glsl_fract);
			bv_library_add_function(lib, "fwidth", lib_glsl_fwidth);
			bv_library_add_function(lib, "fwidthCoarse", lib_glsl_fwidthCoarse);
			bv_library_add_function(lib, "fwidthFine", lib_glsl_fwidthFine);
			bv_library_add_function(lib, "inversesqrt", lib_glsl_inversesqrt);
			bv_library_add_function(lib, "isinf", lib_glsl_isinf);
			bv_library_add_function(lib, "isnan", lib_glsl_isnan);
			bv_library_add_function(lib, "log", lib_glsl_log);
			bv_library_add_function(lib, "log2", lib_glsl_log2);
			bv_library_add_function(lib, "max", lib_glsl_max);
			bv_library_add_function(lib, "min", lib_glsl_min);
			bv_library_add_function(lib, "mix", lib_glsl_mix);
			bv_library_add_function(lib, "mod", lib_glsl_mod);
			bv_library_add_function(lib, "modf", lib_glsl_modf);
			bv_library_add_function(lib, "noise1", lib_glsl_noise1);
			bv_library_add_function(lib, "noise2", lib_glsl_noise2);
			bv_library_add_function(lib, "noise3", lib_glsl_noise3);
			bv_library_add_function(lib, "noise4", lib_glsl_noise4);
			bv_library_add_function(lib, "pow", lib_glsl_noise4);
			bv_library_add_function(lib, "round", lib_glsl_round);
			bv_library_add_function(lib, "roundEven", lib_glsl_roundEven);
			bv_library_add_function(lib, "sign", lib_glsl_sign);
			bv_library_add_function(lib, "smoothstep", lib_glsl_smoothstep);
			bv_library_add_function(lib, "sqrt", lib_glsl_sqrt);
			bv_library_add_function(lib, "step", lib_glsl_step);
			bv_library_add_function(lib, "trunc", lib_glsl_trunc);

			// texture()
			bv_library_add_function(lib, "texture", lib_glsl_texture);

			return lib;
		}
	}
}