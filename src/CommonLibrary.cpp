#include <ShaderDebugger/CommonLibrary.h>
#include <ShaderDebugger/ShaderDebugger.h>
#include <ShaderDebugger/Utils.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>

namespace sd
{
	namespace Common
	{
		bv_variable Discard(bv_program* prog, u8 count, bv_variable* args)
		{
			ShaderDebugger* dbgr = (ShaderDebugger*)prog->user_data;
			dbgr->SetDiscarded(true);
			return bv_variable_create_void();
		}

		bool isGLSL(bv_program* prog)
		{
			auto dbg = (sd::ShaderDebugger*)prog->user_data;
			return dbg->GetCompiler()->GetLanguage() == Compiler::Language::GLSL;
		}

		// initialize user defined structures
		void DefaultConstructor(bv_program* prog, bv_object* obj)
		{
			sd::ShaderDebugger* dbg = (sd::ShaderDebugger*)prog->user_data;

			const std::vector<sd::Structure>& strs = dbg->GetCompiler()->GetStructures();
			const sd::Structure* typeData = nullptr;
			std::string objName(obj->type->name);

			for (const auto& str : strs)
				if (str.Name == objName) {
					typeData = &str;
					break;
				}

			// we can get structure members
			if (typeData != nullptr) {
				bv_object_info* type = obj->type;

				for (u16 i = 0; i < type->props.name_count; i++) {
					std::string propName(type->props.names[i]);
					
					for (const auto& propData : typeData->Members) {
						if (propData.Name == propName) {
							// check for semantic
							if (!propData.Semantic.empty()) {
								bv_variable semValue = dbg->GetSemanticValue(propData.Semantic);
								if (semValue.type != bv_type_void) {
									obj->prop[i] = bv_variable_copy(semValue);
									continue;
								}
							}
							
							bv_object_info* propObjInfo = bv_program_get_object_info(prog, propData.Type.c_str());
							
							// structure
							if (propObjInfo != nullptr) {
								obj->prop[i] = bv_variable_create_object(propObjInfo);
								bv_object* propObj = bv_variable_get_object(obj->prop[i]);

								if (!sd::IsBasicTexture(propObjInfo->name)) {
									if (propObjInfo->ext_method_count > 0) {
										// probably a vector or matrix
										bv_type castType = sd::GetMatrixTypeFromName(propObjInfo->name);
										if (castType == bv_type_void)
											castType = sd::GetVectorTypeFromName(propObjInfo->name);

										for (u16 j = 0; j < propObjInfo->props.name_count; j++)
											propObj->prop[j] = bv_variable_cast(castType, bv_variable_create_float(0.0f));
									}
									else
										DefaultConstructor(prog, propObj);
								}
							}
							// scalar
							else {
								bv_type castType = sd::GetVariableTypeFromName(propData.Type.c_str());
								if (castType == bv_type_void) castType = bv_type_float;

								obj->prop[i] = bv_variable_cast(castType, bv_variable_create_float(0.0f));
							}

							break;
						}
					}
				}
			}
		}

		// TODO: this could've been achieved with templates, bv_type vec_type, u8 components
		bv_variable create_float3(bv_program* prog, glm::vec3 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "vec3" : "float3"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_float(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_float(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_float(val.z));

			return ret;
		}
		bv_variable create_int3(bv_program* prog, glm::ivec3 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "ivec3" : "int3"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_int(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_int(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_int(val.z));

			return ret;
		}
		bv_variable create_uint3(bv_program* prog, glm::uvec3 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "uvec3" : "uint3"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uint(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uint(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_uint(val.z));

			return ret;
		}
		bv_variable create_bool3(bv_program* prog, glm::bvec3 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "bvec3" : "bool3"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uchar(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uchar(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_uchar(val.z));

			return ret;
		}

		bv_variable create_float4(bv_program* prog, glm::vec4 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "vec4" : "float4"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_float(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_float(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_float(val.z));
			bv_object_set_property(retObj, "w", bv_variable_create_float(val.w));

			return ret;
		}
		bv_variable create_int4(bv_program* prog, glm::ivec4 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "ivec4" : "int4"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_int(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_int(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_int(val.z));
			bv_object_set_property(retObj, "w", bv_variable_create_int(val.w));

			return ret;
		}
		bv_variable create_uint4(bv_program* prog, glm::uvec4 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "uvec4" : "uint4"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uint(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uint(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_uint(val.z));
			bv_object_set_property(retObj, "w", bv_variable_create_uint(val.w));

			return ret;
		}
		bv_variable create_bool4(bv_program* prog, glm::bvec4 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "bvec4" : "bool4"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uchar(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uchar(val.y));
			bv_object_set_property(retObj, "z", bv_variable_create_uchar(val.z));
			bv_object_set_property(retObj, "w", bv_variable_create_uchar(val.w));

			return ret;
		}

		bv_variable create_float2(bv_program* prog, glm::vec2 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "vec2" : "float2"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_float(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_float(val.y));

			return ret;
		}
		bv_variable create_int2(bv_program* prog, glm::ivec2 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "ivec2" : "int2"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_int(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_int(val.y));

			return ret;
		}
		bv_variable create_uint2(bv_program* prog, glm::uvec2 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "uvec2" : "uint2"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uint(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uint(val.y));

			return ret;
		}
		bv_variable create_bool2(bv_program* prog, glm::bvec2 val)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, isGLSL(prog) ? "bvec2" : "bool2"));
			bv_object* retObj = bv_variable_get_object(ret);

			bv_object_set_property(retObj, "x", bv_variable_create_uchar(val.x));
			bv_object_set_property(retObj, "y", bv_variable_create_uchar(val.y));

			return ret;
		}

		// TODO: remove all other functions
		bv_variable create_vec(bv_program* prog, bv_type type, u16 components)
		{
			if (components == 3) // 3D vector
			{
				if (type == bv_type_uint)
					return create_uint3(prog);
				else if (type == bv_type_int)
					return create_int3(prog);
				else if (type == bv_type_uchar || type == bv_type_char)
					return create_bool3(prog);
				else
					return create_float3(prog);
			}
			else if (components == 4) // 4D vector
			{
				if (type == bv_type_uint)
					return create_uint4(prog);
				else if (type == bv_type_int)
					return create_int4(prog);
				else if (type == bv_type_uchar || type == bv_type_char)
					return create_bool4(prog);
				else
					return create_float4(prog);
			}
			else if (components == 2) // 2D vector
			{
				if (type == bv_type_uint)
					return create_uint2(prog);
				else if (type == bv_type_int)
					return create_int2(prog);
				else if (type == bv_type_uchar || type == bv_type_char)
					return create_bool2(prog);
				else
					return create_float2(prog);
			}

			return create_float3(prog);
		}
		bv_variable create_mat(bv_program* prog, const char* name, sd::Matrix* mat)
		{
			bv_variable ret = bv_variable_create_object(bv_program_get_object_info(prog, name));
			bv_object* retObj = bv_variable_get_object(ret);

			retObj->user_data = (void*)mat;

			return ret;
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
		
		/* vectors and operators */
		bv_variable lib_common_vec_constructor(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			bv_type type = sd::GetVectorTypeFromName(me->type->name);
			u8 size = me->type->props.name_count;

			u8 propSet = 0;
			for (u8 i = 0; i < count; i++) {
				bv_variable* arg = &args[i];

				// vector
				if (arg->type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(*arg);

					for (u16 i = 0; i < vec->type->props.name_count && i < size; i++) {
						me->prop[propSet] = bv_variable_cast(type, vec->prop[i]);
						propSet++;
					}
				}
				// scalar
				else {
					me->prop[propSet] = bv_variable_cast(type, *arg);
					propSet++;
				}
			}

			// set all other members
			if (propSet == 0 || (propSet != size)) {
				bv_variable val = bv_variable_create_float(0.0f);
				if (propSet != size && propSet != 0)
					val = me->prop[0];

				for (u8 i = propSet; i < size; i++)
					me->prop[i] = bv_variable_cast(type, val);
			}

			return bv_variable_create_void();
		}
		bv_variable lib_common_vec_operator_equal(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_vec_operator_add(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_vec_operator_minus(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_vec_operator_greater(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_vec_operator_divide(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_vec_operator_multiply(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
						glm::vec4 retVec = multiply_mat_vec(*matData, me, isGLSL(prog));
						ret = create_vec(prog, matData->Type, me->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);
						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_cast(matData->Type, bv_variable_create_float(retVec[i]));
					}
				}
			}

			return ret;
		}
		bv_variable lib_common_vec_operator_increment(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			if (count == 0) {
				for (u16 i = 0; i < me->type->props.name_count; i++)
					bv_variable_op_increment(prog, &me->prop[i]);
			}

			return bv_variable_create_void();
		}
		bv_variable lib_common_vec_operator_decrement(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			if (count == 0) {
				for (u16 i = 0; i < me->type->props.name_count; i++)
					bv_variable_op_decrement(prog, &me->prop[i]);
			}

			return bv_variable_create_void();
		}
		bv_variable lib_common_vec_operator_logical_not(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_vec_operator_array_get(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_vec_operator_array_set(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_mat_constructor(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
						float scalar = bv_variable_get_float(bv_variable_cast(data->Type, args[x * data->Columns + y]));
						data->Data[x][y] = scalar;
					}
			}

			return ret;
		}
		void lib_common_mat_destructor(bv_object* me)
		{
			if (me->user_data != 0)
				delete (sd::Matrix*)me->user_data;

			me->user_data = 0;
		}
		void* lib_common_mat_copy(void* udata)
		{
			if (udata != 0)
				return (void*)CopyMatrixData((sd::Matrix*)udata);

			return 0;
		}
		bv_variable lib_common_mat_operator_multiply(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
						sd::Matrix* retMat = CopyMatrixData(myData);
						retMat->Data = retMat->Data * ((sd::Matrix*)obj->user_data)->Data; // elements outside of the matrix should be 0.0f, so we don't care if we just multiply two 4x4 mats

						ret = create_mat(prog, me->type->name, retMat);
					}

					// mat * vec
					else
					{
						glm::vec4 retVec = multiply_mat_vec(*myData, obj, !isGLSL(prog));
						ret = create_vec(prog, myData->Type, obj->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);
						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_cast(myData->Type, bv_variable_create_float(retVec[i]));
					}
				}
			}
			return ret;
		}
		bv_variable lib_common_mat_operator_add(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
						sd::Matrix* retMat = CopyMatrixData(myData);
						retMat->Data = retMat->Data + ((sd::Matrix*)obj->user_data)->Data;

						ret = create_mat(prog, me->type->name, retMat);
					}
				}
			}
			return ret;
		}
		bv_variable lib_common_mat_operator_equal(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_mat_operator_increment(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			if (count == 0)
				((sd::Matrix*)me->user_data)->Data++;

			return bv_variable_create_void();
		}
		bv_variable lib_common_mat_operator_decrement(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
		{
			if (count == 0)
				((sd::Matrix*)me->user_data)->Data--;

			return bv_variable_create_void();
		}
		bv_variable lib_common_mat_operator_array_get(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_mat_operator_array_set(bv_program* prog, bv_object* me, u8 count, bv_variable* args)
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
		bv_variable lib_common_acos(bv_program* prog, u8 count, bv_variable* args)
		{
			/* acos(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // acos(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::acos(sd::AsVector<4, float>(args[0])); // acos takes only float vectors

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_asin(bv_program* prog, u8 count, bv_variable* args)
		{
			/* asin(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // asin(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::asin(sd::AsVector<4, float>(args[0])); // asin takes only float vectors

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_atan(bv_program* prog, u8 count, bv_variable* args)
		{
			/* atan(genType y_over_x) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // atan(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::atan(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else
					return bv_variable_create_float(glm::atan(bv_variable_get_float(args[0])));
			}
			/* atan2(genType y, genType x) */
			else if (count == 2) {
				if (args[0].type == bv_type_object) { // atan(vec3), ...
					bv_object* y = bv_variable_get_object(args[0]);
					glm::vec4 yData = sd::AsVector<4, float>(args[0]);

					bv_object* x = bv_variable_get_object(args[1]);
					glm::vec4 xData = sd::AsVector<4, float>(args[1]);

					bv_variable ret = Common::create_vec(prog, bv_type_float, y->type->props.name_count);
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
		bv_variable lib_common_cos(bv_program* prog, u8 count, bv_variable* args)
		{
			/* cos(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // cos(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::cos(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_cosh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* cosh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // cosh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::cosh(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_sin(bv_program* prog, u8 count, bv_variable* args)
		{
			/* sin(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // sin(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::sin(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_sinh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* sinh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // sinh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::sinh(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_tan(bv_program* prog, u8 count, bv_variable* args)
		{
			/* tan(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // tan(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::tan(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_tanh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* tanh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // tanh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::tanh(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_degrees(bv_program* prog, u8 count, bv_variable* args)
		{
			/* degrees(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // degrees(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::degrees(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_radians(bv_program* prog, u8 count, bv_variable* args)
		{
			/* radians(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // radians(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::radians(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_abs(bv_program* prog, u8 count, bv_variable* args)
		{
			/* abs(genType), abs(genIType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// abs(vec)
					if (vec->prop[0].type == bv_type_float) {
						glm::vec4 vecData = glm::abs(sd::AsVector<4, float>(args[0]));

						bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(vecData[i]);

						return ret;
					}
					// abs(ivec)
					else if (vec->prop[0].type == bv_type_int) {
						glm::ivec4 vecData = glm::abs(sd::AsVector<4, int>(args[0])); // acos takes only float vectors

						bv_variable ret = Common::create_vec(prog, bv_type_int, vec->type->props.name_count);
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
		bv_variable lib_common_ceil(bv_program* prog, u8 count, bv_variable* args)
		{
			/* ceil(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::ceil(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_clamp(bv_program* prog, u8 count, bv_variable* args)
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

						bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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

						bv_variable ret = Common::create_vec(prog, bv_type_uint, vec->type->props.name_count);
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

						bv_variable ret = Common::create_vec(prog, bv_type_int, vec->type->props.name_count);
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
		bv_variable lib_common_dFdx(bv_program* prog, u8 count, bv_variable* args)
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
					return Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdx(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_common_dFdy(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			/* dFdy(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdy(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdy(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_common_dFdxCoarse(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			/* dFdxCoarse(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdxCoarse(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdxCoarse(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_common_dFdyCoarse(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			/* dFdyCoarse(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdyCoarse(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdyCoarse(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_common_dFdxFine(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			/* dFdxFine(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdxFine(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdxFine(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_common_dFdyFine(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			/* dFdyFine(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // dFdyFine(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // dFdyFine(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_common_exp(bv_program* prog, u8 count, bv_variable* args)
		{
			/* exp(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // exp(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::exp(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_exp2(bv_program* prog, u8 count, bv_variable* args)
		{
			/* exp2(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // exp2(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::exp2(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_floor(bv_program* prog, u8 count, bv_variable* args)
		{
			/* floor(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::floor(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_fma(bv_program* prog, u8 count, bv_variable* args)
		{
			// a * b + c
			/* fma(genType, genType, genType), fma(genDType, genDType, genDType)  */
			if (count == 3) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::vec4 a = sd::AsVector<4, float>(args[0]);
					glm::vec4 b = sd::AsVector<4, float>(args[1]);
					glm::vec4 c = sd::AsVector<4, float>(args[2]);

					glm::vec4 vecData = glm::fma(a, b, c);

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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

					return bv_variable_create_float(glm::fma(a, b, c));
				}
			}

			return bv_variable_create_float(0.0f); // floor() must have 1 argument!
		}
		bv_variable lib_common_fract(bv_program* prog, u8 count, bv_variable* args)
		{
			/* fract(genType), fract(genDType)  */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::vec4 vecData = glm::fract(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_fwidth(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO: requires dFdx/y */
			/* fwidth(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // fwidth(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // fwidth(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_common_inversesqrt(bv_program* prog, u8 count, bv_variable* args)
		{
			/* inversesqrt(genType), inversesqrt(genDType)  */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::vec4 vecData = glm::inversesqrt(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_isinf(bv_program* prog, u8 count, bv_variable* args)
		{
			/* isinf(genType), isinf(genDType)  */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::bvec4 vecData = glm::isinf(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_uchar, vec->type->props.name_count);
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
		bv_variable lib_common_isnan(bv_program* prog, u8 count, bv_variable* args)
		{
			/* isnan(genType), isnan(genDType)  */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::bvec4 vecData = glm::isnan(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_uchar, vec->type->props.name_count);
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
		bv_variable lib_common_log(bv_program* prog, u8 count, bv_variable* args)
		{
			/* log(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // log(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::log(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_log2(bv_program* prog, u8 count, bv_variable* args)
		{
			/* log2(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // exp2(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::log2(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_max(bv_program* prog, u8 count, bv_variable* args)
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

						bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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

						bv_variable ret = Common::create_vec(prog, bv_type_uint, vec->type->props.name_count);
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

						bv_variable ret = Common::create_vec(prog, bv_type_int, vec->type->props.name_count);
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
		bv_variable lib_common_min(bv_program* prog, u8 count, bv_variable* args)
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

						bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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

						bv_variable ret = Common::create_vec(prog, bv_type_uint, vec->type->props.name_count);
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

						bv_variable ret = Common::create_vec(prog, bv_type_int, vec->type->props.name_count);
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
		bv_variable lib_common_mix(bv_program* prog, u8 count, bv_variable* args)
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

						bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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

						bv_variable ret = Common::create_vec(prog, bv_type_uint, vec->type->props.name_count);
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

						bv_variable ret = Common::create_vec(prog, bv_type_int, vec->type->props.name_count);
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

						bv_variable ret = Common::create_vec(prog, bv_type_uchar, vec->type->props.name_count);
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
		bv_variable lib_common_modf(bv_program* prog, u8 count, bv_variable* args)
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
					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_pow(bv_program* prog, u8 count, bv_variable* args)
		{
			/* pow(genType y, genType x) */
			if (count == 2) {
				if (args[0].type == bv_type_object) {
					bv_object* x = bv_variable_get_object(args[0]);
					glm::vec4 xData = sd::AsVector<4, float>(args[0]);

					bv_object* y = bv_variable_get_object(args[1]);
					glm::vec4 yData = sd::AsVector<4, float>(args[1]);

					glm::vec4 resData = glm::pow(xData, yData);

					bv_variable ret = Common::create_vec(prog, bv_type_float, y->type->props.name_count);
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
		bv_variable lib_common_round(bv_program* prog, u8 count, bv_variable* args)
		{
			/* round(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::round(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_sign(bv_program* prog, u8 count, bv_variable* args)
		{
			/* sign(genType), sign(genIType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// sign(vec)
					if (vec->prop[0].type == bv_type_float) {
						glm::vec4 vecData = glm::sign(sd::AsVector<4, float>(args[0]));

						bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(vecData[i]);

						return ret;
					}
					// sign(ivec)
					else if (vec->prop[0].type == bv_type_int) {
						glm::ivec4 vecData = glm::sign(sd::AsVector<4, int>(args[0])); // acos takes only float vectors

						bv_variable ret = Common::create_vec(prog, bv_type_int, vec->type->props.name_count);
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
		bv_variable lib_common_smoothstep(bv_program* prog, u8 count, bv_variable* args)
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

					bv_variable ret = Common::create_vec(prog, bv_type_float, vecSize);
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


						bv_variable ret = Common::create_vec(prog, bv_type_float, vecSize);
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
		bv_variable lib_common_sqrt(bv_program* prog, u8 count, bv_variable* args)
		{
			/* sqrt(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // sqrt(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::sqrt(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_step(bv_program* prog, u8 count, bv_variable* args)
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

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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

						bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
						bv_object* retObj = bv_variable_get_object(ret);

						for (u16 i = 0; i < retObj->type->props.name_count; i++)
							retObj->prop[i] = bv_variable_create_float(retData[i]);

						return ret;
					}
					else
						return bv_variable_create_float(
							glm::step(edge.x, bv_variable_get_float(bv_variable_cast(bv_type_float, args[1])))
						);
				}

			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_common_trunc(bv_program* prog, u8 count, bv_variable* args)
		{
			/* trunc(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // trunc(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::trunc(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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

		/* vector */
		bv_variable lib_common_cross(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec3 cross(vec3, vec3) */
			glm::vec3 x = sd::AsVector<3, float>(args[0]);
			glm::vec3 y = sd::AsVector<3, float>(args[1]);

			return Common::create_float3(prog, glm::cross(x, y));
		}
		bv_variable lib_common_distance(bv_program* prog, u8 count, bv_variable* args)
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
		bv_variable lib_common_dot(bv_program* prog, u8 count, bv_variable* args)
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
		bv_variable lib_common_faceforward(bv_program* prog, u8 count, bv_variable* args)
		{
			/* genType faceforward(genType,genType,genType) */
			if (args[0].type == bv_type_object) {
				u16 vecSize = bv_variable_get_object(args[0])->type->props.name_count;

				glm::vec4 n = sd::AsVector<4, float>(args[0]);
				glm::vec4 i = sd::AsVector<4, float>(args[1]);
				glm::vec4 nref = sd::AsVector<4, float>(args[2]);

				glm::vec4 retData = glm::faceforward(n, i, nref);

				bv_variable ret = Common::create_vec(prog, bv_type_float, vecSize);
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
		bv_variable lib_common_length(bv_program* prog, u8 count, bv_variable* args)
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
		bv_variable lib_common_normalize(bv_program* prog, u8 count, bv_variable* args)
		{
			/* genType normalize(genType) */
			if (args[0].type == bv_type_object) {
				glm::vec4 x = sd::AsVector<4, float>(args[0]);
				glm::vec4 retData = glm::normalize(x);

				u16 vecSize = bv_variable_get_object(args[0])->type->props.name_count;
				bv_variable ret = Common::create_vec(prog, bv_type_float, vecSize);
				bv_object* retObj = bv_variable_get_object(ret);
				for (u16 i = 0; i < vecSize; i++)
					retObj->prop[i] = bv_variable_create_float(retData[i]);

				return ret;
			}
			else {
				float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
				return bv_variable_create_float(x);
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_common_reflect(bv_program* prog, u8 count, bv_variable* args)
		{
			/* genType reflect(genType,genType,genType) */
			if (args[0].type == bv_type_object) {
				u16 vecSize = bv_variable_get_object(args[0])->type->props.name_count;

				glm::vec4 i = sd::AsVector<4, float>(args[0]);
				glm::vec4 n = sd::AsVector<4, float>(args[1]);

				glm::vec4 retData = glm::reflect(i, n);

				bv_variable ret = Common::create_vec(prog, bv_type_float, vecSize);
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
		bv_variable lib_common_refract(bv_program* prog, u8 count, bv_variable* args)
		{
			/* genType refract(genType,genType,genType) */
			if (args[0].type == bv_type_object) {
				u16 vecSize = bv_variable_get_object(args[0])->type->props.name_count;

				glm::vec4 i = sd::AsVector<4, float>(args[0]);
				glm::vec4 n = sd::AsVector<4, float>(args[1]);
				float eta = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

				glm::vec4 retData = glm::refract(i, n, eta);

				bv_variable ret = Common::create_vec(prog, bv_type_float, vecSize);
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
		bv_variable lib_common_all(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bool all(bvec) */
			bv_object* x = bv_variable_get_object(args[0]);

			u8 ret = 1;

			for (u8 i = 0; i < x->type->props.name_count; i++)
				ret &= bv_variable_get_uchar(x->prop[i]);

			return bv_variable_create_uchar(ret);
		}
		bv_variable lib_common_any(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bool any(bvec) */
			bv_object* x = bv_variable_get_object(args[0]);

			u8 ret = 0;

			for (u8 i = 0; i < x->type->props.name_count; i++)
				ret |= bv_variable_get_uchar(x->prop[i]);

			return bv_variable_create_uchar(ret);
		}

		/* matrix */
		bv_variable lib_common_determinant(bv_program* prog, u8 count, bv_variable* args)
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
		bv_variable lib_common_transpose(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();

			if (count >= 1) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* mat = bv_variable_get_object(args[0]);
					sd::Matrix* matData = (sd::Matrix*)mat->user_data;

					sd::Matrix* copyData = CopyMatrixData(matData);

					copyData->Columns = matData->Rows;
					copyData->Rows = matData->Columns;

					copyData->Data = glm::transpose(matData->Data);

					bool isDouble = mat->type->name[0] == 'd';
					bool is_glsl = isGLSL(prog);

					std::string newName = (is_glsl ? "mat" : "float") + std::to_string(is_glsl ? copyData->Columns : copyData->Rows);
					if (copyData->Rows != copyData->Columns || !is_glsl)
						newName += "x" + std::to_string(is_glsl ? copyData->Rows : copyData->Columns);
					if (isDouble && is_glsl)
						newName = "d" + newName;

					return Common::create_mat(prog, newName.c_str(), copyData);
				}
			}

			return ret;
		}

		/* integer */
		bv_variable lib_common_bitfieldReverse(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitfieldReverse(genI/UType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::bitfieldReverse(sd::AsVector<4, int>(args[0]));

					bv_type outType = vec->prop[0].type;
					bv_variable ret = Common::create_vec(prog, outType, vec->type->props.name_count);
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
		bv_variable lib_common_bitCount(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitCount(genIType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::bitCount(sd::AsVector<4, int>(args[0]));

					bv_type outType = vec->prop[0].type;

					bv_variable ret = Common::create_vec(prog, outType, vec->type->props.name_count);
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
		bv_variable lib_common_findLSB(bv_program* prog, u8 count, bv_variable* args)
		{
			/* findLSB(genI/UType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::findLSB(sd::AsVector<4, int>(args[0]));

					bv_type outType = vec->prop[0].type;
					bv_variable ret = Common::create_vec(prog, outType, vec->type->props.name_count);
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
		bv_variable lib_common_findMSB(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitfieldReverse(genI/UType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::findMSB(sd::AsVector<4, int>(args[0]));

					bv_type outType = vec->prop[0].type;
					bv_variable ret = Common::create_vec(prog, outType, vec->type->props.name_count);
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

		/* floating point */
		bv_variable lib_common_frexp(bv_program* prog, u8 count, bv_variable* args)
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
					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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
		bv_variable lib_common_ldexp(bv_program* prog, u8 count, bv_variable* args)
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
					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
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

		/* helper functions to create vector & matrix definitions */
		bv_object_info* lib_add_vec(bv_library* lib, const char* name, u8 comp, u8 logNot)
		{
			bv_object_info* vec = bv_object_info_create(name);

			if (comp >= 1) bv_object_info_add_property(vec, "x");
			if (comp >= 2) bv_object_info_add_property(vec, "y");
			if (comp >= 3) bv_object_info_add_property(vec, "z");
			if (comp >= 4) bv_object_info_add_property(vec, "w");

			bv_object_info_add_ext_method(vec, name, lib_common_vec_constructor);

			bv_object_info_add_ext_method(vec, "==", lib_common_vec_operator_equal);
			bv_object_info_add_ext_method(vec, "+", lib_common_vec_operator_add);
			bv_object_info_add_ext_method(vec, "-", lib_common_vec_operator_minus);
			bv_object_info_add_ext_method(vec, ">", lib_common_vec_operator_greater);
			bv_object_info_add_ext_method(vec, "/", lib_common_vec_operator_divide);
			bv_object_info_add_ext_method(vec, "*", lib_common_vec_operator_multiply);
			bv_object_info_add_ext_method(vec, "++", lib_common_vec_operator_increment);
			bv_object_info_add_ext_method(vec, "--", lib_common_vec_operator_decrement);
			bv_object_info_add_ext_method(vec, "[]", lib_common_vec_operator_array_get);
			bv_object_info_add_ext_method(vec, "[]=", lib_common_vec_operator_array_set);
			if (logNot) bv_object_info_add_ext_method(vec, "!", lib_common_vec_operator_logical_not);

			bv_library_add_object_info(lib, vec);

			return vec;
		}
		bv_object_info* lib_add_mat(bv_library* lib, const char* name)
		{
			bv_object_info* mat = bv_object_info_create(name);
			mat->on_delete = lib_common_mat_destructor;
			mat->on_copy = lib_common_mat_copy;

			bv_object_info_add_ext_method(mat, name, lib_common_mat_constructor);

			bv_object_info_add_ext_method(mat, "*", lib_common_mat_operator_multiply);
			bv_object_info_add_ext_method(mat, "+", lib_common_mat_operator_add);
			bv_object_info_add_ext_method(mat, "++", lib_common_mat_operator_increment);
			bv_object_info_add_ext_method(mat, "--", lib_common_mat_operator_decrement);
			bv_object_info_add_ext_method(mat, "==", lib_common_mat_operator_equal);
			bv_object_info_add_ext_method(mat, "[]", lib_common_mat_operator_array_get);
			bv_object_info_add_ext_method(mat, "[]=", lib_common_mat_operator_array_set);

			bv_library_add_object_info(lib, mat);

			return mat;
		}
	}
}