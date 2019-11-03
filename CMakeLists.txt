cmake_minimum_required(VERSION 3.1)
project(ShaderDebugger)

set(CMAKE_MODULE_PATH "./cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)

# source code
set(SOURCES
	main.cpp
	src/Utils.cpp
	src/GLSLLibrary.cpp
    src/GLSLTranslator.cpp
	src/ShaderDebugger.cpp
	
# glsl-parser
	libs/glsl-parser/ast.cpp
	libs/glsl-parser/debug.cpp
	libs/glsl-parser/lexer.cpp
	libs/glsl-parser/parser.cpp
	libs/glsl-parser/util.cpp


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
)

# glm
find_package(GLM REQUIRED)

# cmake toolchain 
if(CMAKE_TOOLCHAIN_FILE)
    include(${CMAKE_TOOLCHAIN_FILE})
endif(CMAKE_TOOLCHAIN_FILE)

# create executable
add_executable(ShaderDebugger ${SOURCES})

# properties
set_target_properties(ShaderDebugger PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# include directories
target_include_directories(ShaderDebugger PRIVATE ${GLM_INCLUDE_DIRS} inc libs libs/BlueVM/inc)

# link libraries
target_link_libraries(ShaderDebugger ${GLM_LIBRARY_DIRS})

if (NOT MSVC)
	target_compile_options(ShaderDebugger PRIVATE -Wno-narrowing)
endif()
