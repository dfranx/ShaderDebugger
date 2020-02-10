cmake_minimum_required(VERSION 3.1)
project(ShaderDebugger)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# source code
set(SOURCES
	src/Utils.cpp
	src/GLSLLibrary.cpp
	src/GLSLCompiler.cpp
	src/HLSLLibrary.cpp
	src/HLSLCompiler.cpp
	src/CommonLibrary.cpp
	src/ShaderDebugger.cpp
	src/Texture.cpp
	
# glsl-parser
	libs/glsl-parser/ast.cpp
	libs/glsl-parser/debug.cpp
	libs/glsl-parser/lexer.cpp
	libs/glsl-parser/parser.cpp
	libs/glsl-parser/util.cpp

# hlslparser
	libs/hlslparser/Engine.cpp
	libs/hlslparser/HLSLParser.cpp
	libs/hlslparser/HLSLTokenizer.cpp
	libs/hlslparser/HLSLTree.cpp

# BlueVM
	libs/BlueVM/src/bv_array.c
	libs/BlueVM/src/bv_block.c
	libs/BlueVM/src/bv_constant_pool.c
	libs/BlueVM/src/bv_function.c
	libs/BlueVM/src/bv_function_pool.c
	libs/BlueVM/src/bv_function_stepper.c
	libs/BlueVM/src/bv_string_table.c
	libs/BlueVM/src/bv_object_info.c
	libs/BlueVM/src/bv_object_pool.c
	libs/BlueVM/src/bv_library.c
	libs/BlueVM/src/bv_object.c
	libs/BlueVM/src/bv_opcode.c
	libs/BlueVM/src/bv_header.c
	libs/BlueVM/src/bv_name_list.c
	libs/BlueVM/src/bv_execute.c
	libs/BlueVM/src/bv_program.c
	libs/BlueVM/src/bv_stack.c
	libs/BlueVM/src/bv_scope.c
	libs/BlueVM/src/bv_type.c
	libs/BlueVM/src/bv_variable.c
	libs/BlueVM/src/types.c

# preprocessor
	libs/wgtcc/ast.cc
	libs/wgtcc/cpp.cc
	libs/wgtcc/encoding.cc
	libs/wgtcc/error.cc
	libs/wgtcc/evaluator.cc
	libs/wgtcc/parser.cc
	libs/wgtcc/scanner.cc
	libs/wgtcc/scope.cc
	libs/wgtcc/token.cc
	libs/wgtcc/type.cc
)

# glm
find_package(GLM REQUIRED)

add_library(ShaderDebugger ${SOURCES})

# link libraries
target_link_libraries(ShaderDebugger ${GLM_LIBRARY_DIRS})

# properties
set_target_properties(ShaderDebugger PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# include directories
target_include_directories(ShaderDebugger PUBLIC inc libs libs/BlueVM/inc ${GLM_INCLUDE_DIRS})