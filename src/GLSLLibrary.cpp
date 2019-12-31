#include <ShaderDebugger/GLSLLibrary.h>
#include <ShaderDebugger/Texture.h>
#include <ShaderDebugger/Matrix.h>
#include <ShaderDebugger/Utils.h>
#include <FastNoise/FastNoise.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>

namespace sd
{
	namespace Library
	{
		/*
			TODO: 
				- dFdx, dFdy, dFdxCoarse, dFdyCoarse, dFdxFine, dFdyFine
				- fwidth, fwidthCoarse, fwidthFine
				- interpolateAtCentroid, interpolateAtOffset, interpolateAtSample, textureGatherOffsets, textureGrad, textureGradOffset, textureProjGrad, textureProjGradOffset, textureQueryLod, textureSamples
				- double type & (un)packDouble2x32
				- LOD calculation, or have a ShaderDebugger::LOD which the user can set?
				- reimplement noise functions so that they follow the "rules"
				- geometry and compute (image, atomic, memory barriers) shader functions
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
				else if (type == bv_type_uchar || type == bv_type_char)
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
				else if (type == bv_type_uchar || type == bv_type_char)
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
				else if (type == bv_type_uchar || type == bv_type_char)
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

		u8 is_basic_texture(const char* name)
		{
			static const std::vector<const char*> names = {
				"isampler1D", "usampler1D", "sampler1D",
				"isampler2D", "usampler2D", "sampler2D",
				"isampler3D", "usampler3D", "sampler3D"
			};
			for (u16 i = 0; i < names.size(); i++)
				if (strcmp(names[i], name) == 0)
					return 1;
			return 0;
		}
		bv_type get_texture_type(const char* name)
		{
			static const std::vector<const char*> namesU = {
				"usampler1D", "usampler2D", "usampler3D"
			};
			static const std::vector<const char*> namesI = {
				"isampler1D", "isampler2D", "isampler3D"
			};
			for (u16 i = 0; i < namesU.size(); i++)
				if (strcmp(namesU[i], name) == 0)
					return bv_type_uint;
			for (u16 i = 0; i < namesI.size(); i++)
				if (strcmp(namesI[i], name) == 0)
					return bv_type_int;
			return bv_type_float;
		}
		u8 get_texture_dimension(const char* name)
		{
			static const std::vector<const char*> names = {
				"isampler1D", "usampler1D", "sampler1D",
				"isampler2D", "usampler2D", "sampler2D",
				"isampler3D", "usampler3D", "sampler3D"
			};
			for (u16 i = 0; i < names.size(); i++)
				if (strcmp(names[i], name) == 0)
					return i/3 + 1;
			return 0;
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

			// printf("[DEBUG] swizzle: %s\n", field);

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
			else if (count == 2) { 
				if (size == 4) {
					bv_object* vec1 = bv_variable_get_object(args[0]);

					// vec4(vec2, vec2)
					if (args[1].type == bv_type_object) {
						bv_object* vec2 = bv_variable_get_object(args[1]);

						me->prop[0] = bv_variable_cast(type, vec1->prop[0]);
						me->prop[1] = bv_variable_cast(type, vec1->prop[1]);
						me->prop[2] = bv_variable_cast(type, vec2->prop[0]);
						me->prop[3] = bv_variable_cast(type, vec2->prop[1]);
					}

					// vec4(vec3, float)
					else {
						me->prop[0] = bv_variable_cast(type, vec1->prop[0]);
						me->prop[1] = bv_variable_cast(type, vec1->prop[1]);
						me->prop[2] = bv_variable_cast(type, vec1->prop[2]);
						me->prop[3] = bv_variable_cast(type, args[1]);
					}
				}
				else if (size == 3) {
					bv_object* vec1 = bv_variable_get_object(args[0]);

					// vec3(vec2, float)
					if (args[1].type != bv_type_object) {
						me->prop[0] = bv_variable_cast(type, vec1->prop[0]);
						me->prop[1] = bv_variable_cast(type, vec1->prop[1]);
						me->prop[2] = bv_variable_cast(type, args[1]);
					}
				}
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
			// --OR--: add a property to ShaderDebugger: glm::vec4 FragmentBlock[4] which then can be used here
			// and user can fill the data however he wants to
			
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
				else {
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

		/* floating-point */
		bv_variable lib_glsl_floatBitsToInt(bv_program* prog, u8 count, bv_variable* args)
		{
			/* floatBitsToInt(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::floatBitsToInt(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_int, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_int(vecData[i]);

					return ret;
				}
				else // floatBitsToInt(scalar)
					return bv_variable_create_int(glm::floatBitsToInt(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_floatBitsToUint(bv_program* prog, u8 count, bv_variable* args)
		{
			/* floatBitsToUint(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::uvec4 vecData = glm::floatBitsToUint(sd::AsVector<4, float>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_uint, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uint(vecData[i]);

					return ret;
				}
				else // floatBitsToUint(scalar)
					return bv_variable_create_uint(glm::floatBitsToUint(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_frexp(bv_program* prog, u8 count, bv_variable* args)
		{
			/* frexp(genType, out genIType), frexp(float, out int), also for genDType */
			if (count == 2) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// using: genType, float, genDType, double
					glm::vec4 x = sd::AsVector<4, float>(args[0]);
					glm::vec4 signData(0.0f);
					glm::ivec4 expValData(0.0f);

					signData = glm::frexp(x, expValData);

					// param out value
					bv_variable* outPtr = bv_variable_get_pointer(args[1]);
					bv_object* outObj = bv_variable_get_object(*outPtr);
					for (u16 i = 0; i < outObj->type->props.name_count; i++)
						outObj->prop[i] = bv_variable_create_int(expValData[i]);

					// return value
					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);
					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(signData[i]);

					return ret;
				}

				// frexp(float, out int)
				else {
					float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					
					int expVal = 0;
					float significand = glm::frexp(x, expVal);

					bv_variable* outPtr = bv_variable_get_pointer(args[1]);
					bv_variable_set_int(outPtr, expVal);

					return bv_variable_create_float(significand);
				}
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_intBitsToFloat(bv_program* prog, u8 count, bv_variable* args)
		{
			/* intBitsToFloat(genIType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::intBitsToFloat(sd::AsVector<4, int>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // intBitsToFloat(scalar)
					return bv_variable_create_float(glm::intBitsToFloat(bv_variable_get_int(bv_variable_cast(bv_type_int, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_uintBitsToFloat(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uintBitsToFloat(genUType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::uintBitsToFloat(sd::AsVector<4, unsigned int>(args[0]));

					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // uintBitsToFloat(scalar)
					return bv_variable_create_float(glm::uintBitsToFloat(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_ldexp(bv_program* prog, u8 count, bv_variable* args)
		{
			/* ldexp(genType, genIType), ldexp(float, int), also for genDType */
			if (count == 2) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// using: genType, float, genDType, double
					glm::vec4 x = sd::AsVector<4, float>(args[0]);
					glm::ivec4 exp = sd::AsVector<4, int>(args[1]);

					glm::vec4 resData = glm::ldexp(x, exp);

					// return value
					bv_variable ret = create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);
					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(resData[i]);

					return ret;
				}

				// ldexp(float, int)
				else {
					float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					int exp = bv_variable_get_int(bv_variable_cast(bv_type_int, args[1]));

					float res = glm::ldexp(x, exp);

					return bv_variable_create_float(res);
				}
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_packDouble2x32(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_packHalf2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uint packHalf2x16(vec2) */
			if (count == 1) {
				if (args[0].type == bv_type_object)
					return bv_variable_create_uint(glm::packHalf2x16(sd::AsVector<2, float>(args[0])));
			}
			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_packUnorm2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uint packUnorm2x16(vec2) */
			if (count == 1) {
				if (args[0].type == bv_type_object)
					return bv_variable_create_uint(glm::packUnorm2x16(sd::AsVector<2, float>(args[0])));
			}
			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_packSnorm2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uint packUnorm2x16(vec2) */
			if (count == 1) {
				if (args[0].type == bv_type_object)
					return bv_variable_create_uint(glm::packSnorm2x16(sd::AsVector<2, float>(args[0])));
			}
			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_packUnorm4x8(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uint packUnorm4x8(vec4) */
			if (count == 1) {
				if (args[0].type == bv_type_object)
					return bv_variable_create_uint(glm::packUnorm4x8(sd::AsVector<4, float>(args[0])));
			}
			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_packSnorm4x8(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uint packSnorm4x8(vec4) */
			if (count == 1) {
				if (args[0].type == bv_type_object)
					return bv_variable_create_uint(glm::packSnorm4x8(sd::AsVector<4, float>(args[0])));
			}
			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_unpackDouble2x32(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			return create_uvec2(prog);
		}
		bv_variable lib_glsl_unpackHalf2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec2 unpackHalf2x16(uint) */
			return create_vec2(prog, glm::unpackHalf2x16(bv_variable_get_uint(args[0])));
		}
		bv_variable lib_glsl_unpackUnorm2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec2 unpackUnorm2x16(uint) */
			return create_vec2(prog, glm::unpackUnorm2x16(bv_variable_get_uint(args[0])));
		}
		bv_variable lib_glsl_unpackSnorm2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec2 unpackSnorm2x16(uint) */
			return create_vec2(prog, glm::unpackSnorm2x16(bv_variable_get_uint(args[0])));
		}
		bv_variable lib_glsl_unpackUnorm4x8(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec2 unpackUnorm4x8(uint) */
			return create_vec4(prog, glm::unpackUnorm4x8(bv_variable_get_uint(args[0])));
		}
		bv_variable lib_glsl_unpackSnorm4x8(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec2 unpackSnorm4x8(uint) */
			return create_vec4(prog, glm::unpackSnorm4x8(bv_variable_get_uint(args[0])));
		}

		/* vector */
		bv_variable lib_glsl_cross(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec3 cross(vec3, vec3) */
			glm::vec3 x = sd::AsVector<3, float>(args[0]);
			glm::vec3 y = sd::AsVector<3, float>(args[1]);

			return create_vec3(prog, glm::cross(x,y));
		}
		bv_variable lib_glsl_distance(bv_program* prog, u8 count, bv_variable* args)
		{
			/* float distance(genType) */
			bv_variable ret = bv_variable_create_float(0.0f);
			if (args[0].type == bv_type_object) {
				glm::vec4 x = sd::AsVector<4, float>(args[0]);
				glm::vec4 y = sd::AsVector<4, float>(args[1]);
				bv_variable_set_float(&ret, glm::distance(x, y));
			}
			else {
				float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
				float y = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));
				bv_variable_set_float(&ret, glm::distance(x, y));
			}

			return ret;
		}
		bv_variable lib_glsl_dot(bv_program* prog, u8 count, bv_variable* args)
		{
			/* float dot(genType) */
			bv_variable ret = bv_variable_create_float(0.0f);
			if (args[0].type == bv_type_object) {
				glm::vec4 x = sd::AsVector<4, float>(args[0]);
				glm::vec4 y = sd::AsVector<4, float>(args[1]);
				bv_variable_set_float(&ret, glm::dot(x, y));
			}
			else {
				float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
				float y = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));
				bv_variable_set_float(&ret, glm::dot(x, y));
			}

			return ret;
		}
		bv_variable lib_glsl_equal(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX equal(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_equal(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_faceforward(bv_program* prog, u8 count, bv_variable* args)
		{
			/* genType faceforward(genType,genType,genType) */
			if (args[0].type == bv_type_object) {
				u16 vecSize = bv_variable_get_object(args[0])->type->props.name_count;

				glm::vec4 n = sd::AsVector<4, float>(args[0]);
				glm::vec4 i = sd::AsVector<4, float>(args[1]);
				glm::vec4 nref = sd::AsVector<4, float>(args[2]);

				glm::vec4 retData = glm::faceforward(n, i, nref);

				bv_variable ret = create_vec(prog, bv_type_float, vecSize);
				bv_object* retObj = bv_variable_get_object(ret);
				for (u16 i = 0; i < vecSize; i++)
					retObj->prop[i] = bv_variable_create_float(retData[i]);

				return ret;
			}
			else {
				float n = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
				float i = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));
				float nref = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

				return bv_variable_create_float(glm::faceforward(n, i, nref));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_length(bv_program* prog, u8 count, bv_variable* args)
		{
			/* float length(genType) */
			bv_variable ret = bv_variable_create_float(0.0f);
			if (args[0].type == bv_type_object) {
				glm::vec4 x = sd::AsVector<4, float>(args[0]);
				bv_variable_set_float(&ret, glm::length(x));
			}
			else {
				float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
				bv_variable_set_float(&ret, glm::length(x));
			}

			return ret;
		}
		bv_variable lib_glsl_normalize(bv_program* prog, u8 count, bv_variable* args)
		{
			/* genType normalize(genType) */
			if (args[0].type == bv_type_object) {
				glm::vec4 x = sd::AsVector<4, float>(args[0]);
				glm::vec4 retData = glm::normalize(x);

				u16 vecSize = bv_variable_get_object(args[0])->type->props.name_count;
				bv_variable ret = create_vec(prog, bv_type_float, vecSize);
				bv_object* retObj = bv_variable_get_object(ret);
				for (u16 i = 0; i < vecSize; i++)
					retObj->prop[i] = bv_variable_create_float(retData[i]);

			}
			else {
				float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
				return bv_variable_create_float(x);
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_notEqual(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX notEqual(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);
			
			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_not_equal(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_reflect(bv_program* prog, u8 count, bv_variable* args)
		{
			/* genType reflect(genType,genType,genType) */
			if (args[0].type == bv_type_object) {
				u16 vecSize = bv_variable_get_object(args[0])->type->props.name_count;

				glm::vec4 i = sd::AsVector<4, float>(args[0]);
				glm::vec4 n = sd::AsVector<4, float>(args[1]);

				glm::vec4 retData = glm::reflect(i,n);

				bv_variable ret = create_vec(prog, bv_type_float, vecSize);
				bv_object* retObj = bv_variable_get_object(ret);
				for (u16 i = 0; i < vecSize; i++)
					retObj->prop[i] = bv_variable_create_float(retData[i]);

				return ret;
			}
			else {
				float i = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
				float n = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));

				return bv_variable_create_float(glm::reflect(i, n));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_refract(bv_program* prog, u8 count, bv_variable* args)
		{
			/* genType refract(genType,genType,genType) */
			if (args[0].type == bv_type_object) {
				u16 vecSize = bv_variable_get_object(args[0])->type->props.name_count;

				glm::vec4 i = sd::AsVector<4, float>(args[0]);
				glm::vec4 n = sd::AsVector<4, float>(args[1]);
				float eta = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

				glm::vec4 retData = glm::refract(i, n, eta);

				bv_variable ret = create_vec(prog, bv_type_float, vecSize);
				bv_object* retObj = bv_variable_get_object(ret);
				for (u16 i = 0; i < vecSize; i++)
					retObj->prop[i] = bv_variable_create_float(retData[i]);

				return ret;
			}
			else {
				float i = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
				float n = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));
				float eta = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

				return bv_variable_create_float(glm::refract(i, n, eta));
			}

			return bv_variable_create_float(0.0f);
		}

		/* component comparison */
		bv_variable lib_glsl_all(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bool all(bvec) */
			bv_object* x = bv_variable_get_object(args[0]);
			
			u8 ret = 1;

			for (u8 i = 0; i < x->type->props.name_count; i++)
				ret &= bv_variable_get_uchar(x->prop[i]);

			return bv_variable_create_uchar(ret);
		}
		bv_variable lib_glsl_any(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bool any(bvec) */
			bv_object* x = bv_variable_get_object(args[0]);

			u8 ret = 0;

			for (u8 i = 0; i < x->type->props.name_count; i++)
				ret |= bv_variable_get_uchar(x->prop[i]);

			return bv_variable_create_uchar(ret);
		}
		bv_variable lib_glsl_greaterThan(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX greaterThan(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_greater_than(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_greaterThanEqual(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX greaterThan(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_greater_equal(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_lessThan(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX lessThan(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_less_than(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_lessThanEqual(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX lessThanEqual(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_less_equal(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_not(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX greaterThan(bvecX) */
			bv_object* x = bv_variable_get_object(args[0]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(!bv_variable_get_uchar(x->prop[i]));

			return ret;
		}

		/* texture */
		bv_variable lib_glsl_texelFetch(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// sampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::ivec3 uv = sd::AsVector<3, int>(args[1]);
						int lod = 0;
						if (count >= 3)
							lod = bv_variable_get_int(bv_variable_cast(bv_type_int, args[2]));

						glm::vec4 sample = tex->TexelFetch(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_texelFetchOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 4) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// sampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::ivec3 uv = sd::AsVector<3, int>(args[1]);
						int lod = bv_variable_get_int(bv_variable_cast(bv_type_int, args[2]));
						int offset = bv_variable_get_int(bv_variable_cast(bv_type_int, args[3]));

						glm::vec4 sample = tex->TexelFetch(uv.x + offset, uv.y + offset * (tex->Height > 1), uv.z + offset * (tex->Depth > 1), lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_texture(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec3 uv = sd::AsVector<3, float>(args[1]);

						float lod = 0.0f; // TODO: LOD calculation ?

						float bias = 0.0f;
						if (count >= 3)
							bias = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));
						lod+=floor(bias);

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureGather(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler2D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec2 uv = sd::AsVector<2, float>(args[1]);

						int comp = 0;
						if (count >= 3)
							comp = bv_variable_get_int(bv_variable_cast(bv_type_int, args[2]));

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;

						// TODO: LOD calculation

						glm::vec4 sample_i0_j1 = tex->Sample(uv.x, uv.y + yPixel, 0.0f, 0.0f);
						glm::vec4 sample_i1_j1 = tex->Sample(uv.x + xPixel, uv.y + yPixel, 0.0f, 0.0f);
						glm::vec4 sample_i1_j0 = tex->Sample(uv.x + xPixel, uv.y, 0.0f, 0.0f);
						glm::vec4 sample_i0_j0 = tex->Sample(uv.x, uv.y, 0.0f, 0.0f);

						glm::vec4 sample(0.0f);

						if (comp == 1)
							sample = glm::vec4(sample_i0_j1.y, sample_i1_j1.y, sample_i1_j0.y, sample_i0_j0.y);
						else if (comp == 2)
							sample = glm::vec4(sample_i0_j1.z, sample_i1_j1.z, sample_i1_j0.z, sample_i0_j0.z);
						else if (comp == 3)
							sample = glm::vec4(sample_i0_j1.w, sample_i1_j1.w, sample_i1_j0.w, sample_i0_j0.w);
						else
							sample = glm::vec4(sample_i0_j1.x, sample_i1_j1.x, sample_i1_j0.x, sample_i0_j0.x);

						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureGatherOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 3) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler2D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec2 uv = sd::AsVector<2, float>(args[1]);
						glm::ivec2 offset = sd::AsVector<2, int>(args[2]);

						int comp = 0;
						if (count >= 4)
							comp = bv_variable_get_int(bv_variable_cast(bv_type_int, args[3]));

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;

						uv.x += offset.x * xPixel; // ?
						uv.y += offset.y * yPixel; // ?

						glm::vec4 sample_i0_j1 = tex->Sample(uv.x, uv.y + yPixel, 0.0f, 0.0f);
						glm::vec4 sample_i1_j1 = tex->Sample(uv.x + xPixel, uv.y + yPixel, 0.0f, 0.0f);
						glm::vec4 sample_i1_j0 = tex->Sample(uv.x + xPixel, uv.y, 0.0f, 0.0f);
						glm::vec4 sample_i0_j0 = tex->Sample(uv.x, uv.y, 0.0f, 0.0f);

						glm::vec4 sample(0.0f);

						if (comp == 1)
							sample = glm::vec4(sample_i0_j1.y, sample_i1_j1.y, sample_i1_j0.y, sample_i0_j0.y);
						else if (comp == 2)
							sample = glm::vec4(sample_i0_j1.z, sample_i1_j1.z, sample_i1_j0.z, sample_i0_j0.z);
						else if (comp == 3)
							sample = glm::vec4(sample_i0_j1.w, sample_i1_j1.w, sample_i1_j0.w, sample_i0_j0.w);
						else
							sample = glm::vec4(sample_i0_j1.x, sample_i1_j1.x, sample_i1_j0.x, sample_i0_j0.x);

						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureGatherOffsets(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			if (count >= 1) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) {
					bv_object* sampler = bv_variable_get_object(args[0]);
					create_vec(prog, get_texture_type(sampler->type->name), 4);
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureGrad(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			if (count >= 1) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) {
					bv_object* sampler = bv_variable_get_object(args[0]);
					create_vec(prog, get_texture_type(sampler->type->name), 4);
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureGradOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			if (count >= 1) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) {
					bv_object* sampler = bv_variable_get_object(args[0]);
					create_vec(prog, get_texture_type(sampler->type->name), 4);
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureLod(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 3) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec3 uv = sd::AsVector<3, float>(args[1]);

						float lod = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureLodOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 4) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec3 uv = sd::AsVector<3, float>(args[1]);
						float lod = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));
						glm::ivec3 offset = sd::AsVector<3, int>(args[3]);

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;
						float zPixel = 1.0f / tex->Depth;

						uv.x += offset.x * xPixel;
						if (tex->Height > 1)
							uv.y += offset.y * yPixel;
						if (tex->Depth > 1)
							uv.z += offset.z * zPixel;

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 3) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec3 uv = sd::AsVector<3, float>(args[1]);
						glm::ivec3 offset = sd::AsVector<3, int>(args[2]);

						float lod = 0.0f; // TODO: LOD calculation ?

						float bias = 0.0f;
						if (count >= 3)
							bias = bv_variable_get_float(bv_variable_cast(bv_type_float, args[3]));
						lod += floor(bias);

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;
						float zPixel = 1.0f / tex->Depth;

						uv.x += offset.x * xPixel;
						if (tex->Height > 1)
							uv.y += offset.y * yPixel;
						if (tex->Depth > 1)
							uv.z += offset.z * zPixel;

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureProj(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec4 uv = sd::AsVector<4, float>(args[1]);

						float lod = 0.0f; // TODO: LOD calculation ?

						float bias = 0.0f;
						if (count >= 3)
							bias = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));
						lod += floor(bias);

						u16 vecSize = bv_variable_get_object(args[1])->type->props.name_count;
						if (vecSize == 2)
							uv.x /= uv.y;
						else if (vecSize == 3) {
							uv.x /= uv.z;
							uv.y /= uv.z;
						}
						else if (vecSize == 4) {
							uv.x /= uv.w;
							uv.y /= uv.w;
							uv.z /= uv.w;
						}

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureProjGrad(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			if (count >= 1) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) {
					bv_object* sampler = bv_variable_get_object(args[0]);
					create_vec(prog, get_texture_type(sampler->type->name), 4);
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureProjGradOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			if (count >= 1) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) {
					bv_object* sampler = bv_variable_get_object(args[0]);
					create_vec(prog, get_texture_type(sampler->type->name), 4);
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureProjLod(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 3) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec4 uv = sd::AsVector<4, float>(args[1]);

						float lod = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));
						
						u16 vecSize = bv_variable_get_object(args[1])->type->props.name_count;
						if (vecSize == 2)
							uv.x /= uv.y;
						else if (vecSize == 3) {
							uv.x /= uv.z;
							uv.y /= uv.z;
						}
						else if (vecSize == 4) {
							uv.x /= uv.w;
							uv.y /= uv.w;
							uv.z /= uv.w;
						}

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureProjLodOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 4) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec4 uv = sd::AsVector<4, float>(args[1]);
						glm::ivec3 offset = sd::AsVector<4, int>(args[3]);

						float lod = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;
						float zPixel = 1.0f / tex->Depth;

						uv.x += offset.x * xPixel;
						if (tex->Height > 1)
							uv.y += offset.y * yPixel;
						if (tex->Depth > 1)
							uv.z += offset.z * zPixel;

						u16 vecSize = bv_variable_get_object(args[1])->type->props.name_count;
						if (vecSize == 2)
							uv.x /= uv.y;
						else if (vecSize == 3) {
							uv.x /= uv.z;
							uv.y /= uv.z;
						}
						else if (vecSize == 4) {
							uv.x /= uv.w;
							uv.y /= uv.w;
							uv.z /= uv.w;
						}

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureProjOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 3) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec4 uv = sd::AsVector<4, float>(args[1]);
						glm::ivec3 offset = sd::AsVector<3, int>(args[2]);

						float lod = 0.0f; // TODO: LOD calculation ?

						float bias = 0.0f;
						if (count >= 4)
							bias = bv_variable_get_float(bv_variable_cast(bv_type_float, args[3]));
						lod += floor(bias);

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;
						float zPixel = 1.0f / tex->Depth;

						uv.x += offset.x * xPixel;
						if (tex->Height > 1)
							uv.y += offset.y * yPixel;
						if (tex->Depth > 1)
							uv.z += offset.z * zPixel;

						u16 vecSize = bv_variable_get_object(args[1])->type->props.name_count;
						if (vecSize == 2)
							uv.x /= uv.y;
						else if (vecSize == 3) {
							uv.x /= uv.z;
							uv.y /= uv.z;
						}
						else if (vecSize == 4) {
							uv.x /= uv.w;
							uv.y /= uv.w;
							uv.z /= uv.w;
						}

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return create_ivec4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return create_uvec4(prog, glm::uvec4(sample));

						return create_vec4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return create_vec4(prog);
		}
		bv_variable lib_glsl_textureQueryLevels(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 1) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						return bv_variable_create_int(tex->MipmapLevels);
					}
					/* else if samplerCube ... */
				}
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_textureQueryLod(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			return create_vec2(prog);
		}
		bv_variable lib_glsl_textureSamples(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			return bv_variable_create_int(1);
		}
		bv_variable lib_glsl_textureSize(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (is_basic_texture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						float lod = bv_variable_get_int(bv_variable_cast(bv_type_int, args[1]));

						// TODO: take LOD into account

						if (get_texture_dimension(sampler->type->name) == 1)
							return bv_variable_create_int(tex->Width);
						else if (get_texture_dimension(sampler->type->name) == 3)
							return create_ivec3(prog, glm::ivec3(tex->Width, tex->Height, tex->Depth));
						else
							return create_ivec2(prog, glm::ivec2(tex->Width, tex->Height));
					}
					/* else if samplerCube ... */
				}
			}

			return create_ivec2(prog);
		}

		/* matrix */
		bv_variable lib_glsl_determinant(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_float(0.0f);

			if (count >= 1) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* mat = bv_variable_get_object(args[0]);
					sd::Matrix* matData = (sd::Matrix*)mat->user_data;

					if (matData->Rows == 2)
						bv_variable_set_float(&ret, glm::determinant(glm::mat2(matData->Data)));
					else if (matData->Rows == 3)
						bv_variable_set_float(&ret, glm::determinant(glm::mat3(matData->Data)));
					else
						bv_variable_set_float(&ret, glm::determinant(matData->Data));
				}
			}

			return ret;
		}
		bv_variable lib_glsl_inverse(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();

			if (count >= 1) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* mat = bv_variable_get_object(args[0]);
					sd::Matrix* matData = (sd::Matrix*)mat->user_data;

					sd::Matrix* copyData = copy_mat_data(matData);

					if (matData->Rows == 2)
						copyData->Data = glm::inverse(glm::mat2(matData->Data));
					else if (matData->Rows == 3)
						copyData->Data = glm::inverse(glm::mat3(matData->Data));
					else
						copyData->Data = glm::inverse(matData->Data);

					return create_mat(prog, mat->type->name, copyData);
				}
			}

			return ret;
		}
		bv_variable lib_glsl_matrixCompMult(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();

			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* x = bv_variable_get_object(args[0]);
					sd::Matrix* xData = (sd::Matrix*)x->user_data;

					bv_object* y = bv_variable_get_object(args[1]);
					sd::Matrix* yData = (sd::Matrix*)y->user_data;

					sd::Matrix* copyData = copy_mat_data(xData);

					for (int x = 0; x < xData->Columns; x++) 
						for (int y = 0; y < xData->Rows; y++)
							copyData->Data[x][y] = yData->Data[x][y] * xData->Data[x][y];

					return create_mat(prog, x->type->name, copyData);
				}
			}

			return ret;
		}
		bv_variable lib_glsl_outerProduct(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();

			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* x = bv_variable_get_object(args[0]);
					bv_object* y = bv_variable_get_object(args[1]);

					glm::vec4 xData = sd::AsVector<4, float>(args[0]);
					glm::vec4 yData = sd::AsVector<4, float>(args[1]);

					glm::mat4 resDat = glm::outerProduct(xData, yData);
					int resCols = y->type->props.name_count;
					int resRows = x->type->props.name_count;

					bool isDouble = x->type->name[0] == 'd';

					std::string newName = "mat" + std::to_string(resCols);
					if (resCols != resRows)
						newName += "x" + std::to_string(resRows);
					if (isDouble)
						newName = "d" + newName;

					sd::Matrix* res = new sd::Matrix();
					res->Columns = resCols;
					res->Rows = resRows;
					res->Data = resDat;
					res->Type = bv_type_float;

					return create_mat(prog, newName.c_str(), res);
				}
			}

			return ret;
		}
		bv_variable lib_glsl_transpose(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();

			if (count >= 1) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* mat = bv_variable_get_object(args[0]);
					sd::Matrix* matData = (sd::Matrix*)mat->user_data;

					sd::Matrix* copyData = copy_mat_data(matData);

					copyData->Columns = matData->Rows;
					copyData->Rows = matData->Columns;
					
					copyData->Data = glm::transpose(matData->Data);

					bool isDouble = mat->type->name[0] == 'd';

					std::string newName = "mat" + std::to_string(copyData->Columns);
					if (copyData->Rows != copyData->Columns)
						newName += "x" + std::to_string(copyData->Rows);
					if (isDouble)
						newName = "d" + newName;

					return create_mat(prog, newName.c_str(), copyData);
				}
			}

			return ret;
		}

		/* integer */
		bv_variable lib_glsl_bitCount(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitCount(genIType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::bitCount(sd::AsVector<4, int>(args[0]));

					bv_type outType = vec->prop[0].type;

					bv_variable ret = create_vec(prog, outType, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_cast(outType, bv_variable_create_int(vecData[i]));
					
					return ret;
				}
				// bitCount(scalar)
				else if (args[0].type == bv_type_int)
					return bv_variable_create_int(glm::bitCount(bv_variable_get_int(args[0])));
				// bitCount(scalar)
				else
					return bv_variable_create_uint(glm::bitCount(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]))));
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_bitfieldExtract(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitfieldExtract(genIType, int, int) */
			if (count >= 3) {
				int offset = bv_variable_get_int(args[1]);
				int bits = bv_variable_get_int(args[2]);

				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::bitfieldExtract(sd::AsVector<4, int>(args[0]), offset, bits);

					bv_type outType = vec->prop[0].type;
					bv_variable ret = create_vec(prog, outType, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_cast(outType, bv_variable_create_int(vecData[i]));

					return ret;
				}
				// bitfieldExtract(scalar)
				else if (args[0].type == bv_type_int)
					return bv_variable_create_int(glm::bitfieldExtract(bv_variable_get_int(args[0]), offset, bits));
				// bitfieldExtract(scalar)
				else
					return bv_variable_create_uint(glm::bitfieldExtract(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0])), offset, bits));
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_bitfieldInsert(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitfieldInsert(genI/UType, genI/UType, int, int) */
			if (count >= 4) {
				int offset = bv_variable_get_int(args[1]);
				int bits = bv_variable_get_int(args[2]);

				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::ivec4 base = sd::AsVector<4, int>(args[0]);
					glm::ivec4 insert = sd::AsVector<4, int>(args[1]);

					glm::ivec4 vecData = glm::bitfieldInsert(base, insert, offset, bits);

					bv_type outType = vec->prop[0].type;
					bv_variable ret = create_vec(prog, outType, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_cast(outType, bv_variable_create_int(vecData[i]));

					return ret;
				}
				// bitfieldInsert(scalar)
				else if (args[0].type == bv_type_int) {
					int base = bv_variable_get_int(args[0]);
					int insert = bv_variable_get_int(bv_variable_cast(bv_type_int, args[1]));

					return bv_variable_create_int(glm::bitfieldInsert(base, insert, offset, bits));
				}
				// bitfieldInsert(scalar)
				else if (args[0].type == bv_type_int) {
					unsigned int base = bv_variable_get_uint(args[0]);
					unsigned int insert = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[1]));

					return bv_variable_create_uint(glm::bitfieldInsert(base, insert, offset, bits));
				}
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_bitfieldReverse(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitfieldReverse(genI/UType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::bitfieldReverse(sd::AsVector<4, int>(args[0]));

					bv_type outType = vec->prop[0].type;
					bv_variable ret = create_vec(prog, outType, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_cast(outType, bv_variable_create_int(vecData[i]));

					return ret;
				}
				// bitfieldReverse(scalar)
				else if (args[0].type == bv_type_int)
					return bv_variable_create_int(glm::bitfieldReverse(bv_variable_get_int(args[0])));
				// bitfieldReverse(scalar)
				else
					return bv_variable_create_uint(glm::bitfieldReverse(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]))));
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_findLSB(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitfieldReverse(genI/UType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::findLSB(sd::AsVector<4, int>(args[0]));

					bv_type outType = vec->prop[0].type;
					bv_variable ret = create_vec(prog, outType, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_cast(outType, bv_variable_create_int(vecData[i]));

					return ret;
				}
				// bitfieldReverse(scalar)
				else if (args[0].type == bv_type_int)
					return bv_variable_create_int(glm::findLSB(bv_variable_get_int(args[0])));
				// bitfieldReverse(scalar)
				else
					return bv_variable_create_uint(glm::findLSB(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]))));
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_findMSB(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitfieldReverse(genI/UType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::findMSB(sd::AsVector<4, int>(args[0]));

					bv_type outType = vec->prop[0].type;
					bv_variable ret = create_vec(prog, outType, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_cast(outType, bv_variable_create_int(vecData[i]));

					return ret;
				}
				// bitfieldReverse(scalar)
				else if (args[0].type == bv_type_int)
					return bv_variable_create_int(glm::findMSB(bv_variable_get_int(args[0])));
				// bitfieldReverse(scalar)
				else
					return bv_variable_create_uint(glm::findMSB(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]))));
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_uaddCarry(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uaddCarry */
			if (count >= 3) {
				if (args[0].type == bv_type_object) {
					bv_object* x = bv_variable_get_object(args[0]);
					glm::uvec4 xData = sd::AsVector<4, unsigned int>(args[0]);
					glm::uvec4 yData = sd::AsVector<4, unsigned int>(args[1]);

					glm::uvec4 carry;
					glm::uvec4 res = glm::uaddCarry(xData, yData, carry);

					bv_variable* outCarry = bv_variable_get_pointer(args[2]);
					bv_object* outCarryObj = bv_variable_get_object(*outCarry);
					for (u16 i = 0; i < outCarryObj->type->props.name_count; i++)
						outCarryObj->prop[i] = bv_variable_create_uint(carry[i]);

					bv_variable ret = create_vec(prog, bv_type_uint, outCarryObj->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);
					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uint(res[i]);

					return ret;
				}
				else {
					unsigned int x = bv_variable_get_uint(args[0]);
					unsigned int y = bv_variable_get_uint(args[1]);

					unsigned int res = 0, carry = 0;
					res = glm::uaddCarry(x, y, carry);

					bv_variable* outCarry = bv_variable_get_pointer(args[2]);
					bv_variable_set_uint(outCarry, carry);

					return bv_variable_create_uint(res);
				}
			}

			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_umulExtended(bv_program* prog, u8 count, bv_variable* args)
		{
			/* umulExtended */
			if (count >= 4) {
				if (args[0].type == bv_type_object) {
					glm::uvec4 xData = sd::AsVector<4, unsigned int>(args[0]);
					glm::uvec4 yData = sd::AsVector<4, unsigned int>(args[1]);

					glm::uvec4 msb, lsb;
					glm::umulExtended(xData, yData, msb, lsb);

					bv_variable* outMSB = bv_variable_get_pointer(args[2]);
					bv_object* outMSBObj = bv_variable_get_object(*outMSB);
					for (u16 i = 0; i < outMSBObj->type->props.name_count; i++)
						outMSBObj->prop[i] = bv_variable_create_uint(msb[i]);

					bv_variable* outLSB = bv_variable_get_pointer(args[3]);
					bv_object* outLSBObj = bv_variable_get_object(*outLSB);
					for (u16 i = 0; i < outLSBObj->type->props.name_count; i++)
						outLSBObj->prop[i] = bv_variable_create_uint(lsb[i]);
				}
				else {
					unsigned int x = bv_variable_get_uint(args[0]);
					unsigned int y = bv_variable_get_uint(args[1]);

					unsigned int msb = 0, lsb = 0;
					glm::umulExtended(x, y, msb, lsb);

					bv_variable* outMSB = bv_variable_get_pointer(args[2]);
					bv_variable_set_uint(outMSB, msb);

					bv_variable* outLSB = bv_variable_get_pointer(args[3]);
					bv_variable_set_uint(outLSB, lsb);
				}
			}

			return bv_variable_create_void();
		}
		bv_variable lib_glsl_imulExtended(bv_program* prog, u8 count, bv_variable* args)
		{
			/* imulExtended */
			if (count >= 4) {
				if (args[0].type == bv_type_object) {
					glm::ivec4 xData = sd::AsVector<4, int>(args[0]);
					glm::ivec4 yData = sd::AsVector<4, int>(args[1]);

					glm::ivec4 msb, lsb;
					glm::imulExtended(xData, yData, msb, lsb);

					bv_variable* outMSB = bv_variable_get_pointer(args[2]);
					bv_object* outMSBObj = bv_variable_get_object(*outMSB);
					for (u16 i = 0; i < outMSBObj->type->props.name_count; i++)
						outMSBObj->prop[i] = bv_variable_create_int(msb[i]);

					bv_variable* outLSB = bv_variable_get_pointer(args[3]);
					bv_object* outLSBObj = bv_variable_get_object(*outLSB);
					for (u16 i = 0; i < outLSBObj->type->props.name_count; i++)
						outLSBObj->prop[i] = bv_variable_create_int(lsb[i]);
				}
				else {
					int x = bv_variable_get_int(args[0]);
					int y = bv_variable_get_int(args[1]);

					int msb = 0, lsb = 0;
					glm::imulExtended(x, y, msb, lsb);

					bv_variable* outMSB = bv_variable_get_pointer(args[2]);
					bv_variable_set_int(outMSB, msb);

					bv_variable* outLSB = bv_variable_get_pointer(args[3]);
					bv_variable_set_int(outLSB, lsb);
				}
			}

			return bv_variable_create_void();
		}
		bv_variable lib_glsl_usubBorrow(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uaddCarry */
			if (count >= 3) {
				if (args[0].type == bv_type_object) {
					bv_object* x = bv_variable_get_object(args[0]);
					glm::uvec4 xData = sd::AsVector<4, unsigned int>(args[0]);
					glm::uvec4 yData = sd::AsVector<4, unsigned int>(args[1]);

					glm::uvec4 borrow;
					glm::uvec4 res = glm::usubBorrow(xData, yData, borrow);

					bv_variable* outBorrow = bv_variable_get_pointer(args[2]);
					bv_object* outBorrowObj = bv_variable_get_object(*outBorrow);
					for (u16 i = 0; i < outBorrowObj->type->props.name_count; i++)
						outBorrowObj->prop[i] = bv_variable_create_uint(borrow[i]);

					bv_variable ret = create_vec(prog, bv_type_uint, outBorrowObj->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);
					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uint(res[i]);

					return ret;
				}
				else {
					unsigned int x = bv_variable_get_uint(args[0]);
					unsigned int y = bv_variable_get_uint(args[1]);

					unsigned int res = 0, borrow = 0;
					res = glm::usubBorrow(x, y, borrow);

					bv_variable* outBorrow = bv_variable_get_pointer(args[2]);
					bv_variable_set_uint(outBorrow, borrow);

					return bv_variable_create_uint(res);
				}
			}

			return bv_variable_create_uint(0);
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
			bv_library_add_function(lib, "sin", lib_glsl_sin);
			bv_library_add_function(lib, "sinh", lib_glsl_sinh);
			bv_library_add_function(lib, "tan", lib_glsl_tan);
			bv_library_add_function(lib, "tanh", lib_glsl_tanh);
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

			// floating-point functions
			bv_library_add_function(lib, "floatBitsToInt", lib_glsl_floatBitsToInt);
			bv_library_add_function(lib, "floatBitsToUint", lib_glsl_floatBitsToUint);
			bv_library_add_function(lib, "frexp", lib_glsl_frexp);
			bv_library_add_function(lib, "intBitsToFloat", lib_glsl_intBitsToFloat);
			bv_library_add_function(lib, "uintBitsToFloat", lib_glsl_uintBitsToFloat);
			bv_library_add_function(lib, "ldexp", lib_glsl_ldexp);
			bv_library_add_function(lib, "packDouble2x32", lib_glsl_packDouble2x32);
			bv_library_add_function(lib, "packHalf2x16", lib_glsl_packHalf2x16);
			bv_library_add_function(lib, "packUnorm2x16", lib_glsl_packUnorm2x16);
			bv_library_add_function(lib, "packSnorm2x16", lib_glsl_packSnorm2x16);
			bv_library_add_function(lib, "packUnorm4x8", lib_glsl_packUnorm4x8);
			bv_library_add_function(lib, "packSnorm4x8", lib_glsl_packSnorm4x8);
			bv_library_add_function(lib, "unpackDouble2x32", lib_glsl_unpackDouble2x32);
			bv_library_add_function(lib, "unpackHalf2x16", lib_glsl_unpackHalf2x16);
			bv_library_add_function(lib, "unpackUnorm2x16", lib_glsl_unpackUnorm2x16);
			bv_library_add_function(lib, "unpackSnorm2x16", lib_glsl_unpackSnorm2x16);
			bv_library_add_function(lib, "unpackUnorm4x8", lib_glsl_unpackUnorm4x8);
			bv_library_add_function(lib, "unpackSnorm4x8", lib_glsl_unpackSnorm4x8);
			
			// vector
			bv_library_add_function(lib, "cross", lib_glsl_cross);
			bv_library_add_function(lib, "distance", lib_glsl_distance);
			bv_library_add_function(lib, "dot", lib_glsl_dot);
			bv_library_add_function(lib, "equal", lib_glsl_equal);
			bv_library_add_function(lib, "faceforward", lib_glsl_faceforward);
			bv_library_add_function(lib, "length", lib_glsl_length);
			bv_library_add_function(lib, "normalize", lib_glsl_normalize);
			bv_library_add_function(lib, "notEqual", lib_glsl_notEqual);
			bv_library_add_function(lib, "reflect", lib_glsl_reflect);
			bv_library_add_function(lib, "refract", lib_glsl_refract);

			// component comparison
			bv_library_add_function(lib, "all", lib_glsl_all);
			bv_library_add_function(lib, "any", lib_glsl_any);
			bv_library_add_function(lib, "greaterThan", lib_glsl_greaterThan);
			bv_library_add_function(lib, "greaterThanEqual", lib_glsl_greaterThanEqual);
			bv_library_add_function(lib, "lessThan", lib_glsl_lessThan);
			bv_library_add_function(lib, "lessThanEqual", lib_glsl_lessThanEqual);
			bv_library_add_function(lib, "not", lib_glsl_not);

			// texture
			bv_library_add_function(lib, "texelFetch", lib_glsl_texelFetch);
			bv_library_add_function(lib, "texelFetchOffset", lib_glsl_texelFetchOffset);
			bv_library_add_function(lib, "texture", lib_glsl_texture);
			bv_library_add_function(lib, "textureGather", lib_glsl_textureGather);
			bv_library_add_function(lib, "textureGatherOffset", lib_glsl_textureGatherOffset);
			bv_library_add_function(lib, "textureGatherOffsets", lib_glsl_textureGatherOffsets);
			bv_library_add_function(lib, "textureGrad", lib_glsl_textureGrad);
			bv_library_add_function(lib, "textureGradOffset", lib_glsl_textureGradOffset);
			bv_library_add_function(lib, "textureLod", lib_glsl_textureLod);
			bv_library_add_function(lib, "textureLod", lib_glsl_textureLodOffset);
			bv_library_add_function(lib, "textureOffset", lib_glsl_textureOffset);
			bv_library_add_function(lib, "textureProj", lib_glsl_textureProj);
			bv_library_add_function(lib, "textureProjGrad", lib_glsl_textureProjGrad);
			bv_library_add_function(lib, "textureProjGradOffset", lib_glsl_textureProjGradOffset);
			bv_library_add_function(lib, "textureProjLod", lib_glsl_textureProjLod);
			bv_library_add_function(lib, "textureProjLodOffset", lib_glsl_textureProjLodOffset);
			bv_library_add_function(lib, "textureProjOffset", lib_glsl_textureProjOffset);
			bv_library_add_function(lib, "textureQueryLevels", lib_glsl_textureQueryLevels);
			bv_library_add_function(lib, "textureQueryLod", lib_glsl_textureQueryLod);
			bv_library_add_function(lib, "textureSamples", lib_glsl_textureSamples);
			bv_library_add_function(lib, "textureSize", lib_glsl_textureSize);

			// matrix
			bv_library_add_function(lib, "determinant", lib_glsl_determinant);
			bv_library_add_function(lib, "inverse", lib_glsl_inverse);
			bv_library_add_function(lib, "matrixCompMult", lib_glsl_matrixCompMult);
			bv_library_add_function(lib, "outerProduct", lib_glsl_outerProduct);
			bv_library_add_function(lib, "transpose", lib_glsl_transpose);

			// integer
			bv_library_add_function(lib, "bitCount", lib_glsl_bitCount);
			bv_library_add_function(lib, "bitfieldExtract", lib_glsl_bitfieldExtract);
			bv_library_add_function(lib, "bitfieldInsert", lib_glsl_bitfieldInsert);
			bv_library_add_function(lib, "bitfieldReverse", lib_glsl_bitfieldReverse);
			bv_library_add_function(lib, "findLSB", lib_glsl_findLSB);
			bv_library_add_function(lib, "findMSB", lib_glsl_findMSB);
			bv_library_add_function(lib, "uaddCarry", lib_glsl_uaddCarry);
			bv_library_add_function(lib, "umulExtended", lib_glsl_umulExtended);
			bv_library_add_function(lib, "imulExtended", lib_glsl_imulExtended);
			bv_library_add_function(lib, "usubBorrow", lib_glsl_usubBorrow);

			return lib;
		}
	}
}