cmake_minimum_required(VERSION 3.1)
project(ShaderDebugger)

set(CMAKE_MODULE_PATH "./cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)

# source code
set(SOURCES
    main.cpp
    src/BVMGenerator.cpp
    src/ShaderConverter.cpp
    src/ShaderDebugger.cpp
)

# cmake toolchain 
if(CMAKE_TOOLCHAIN_FILE)
    include(${CMAKE_TOOLCHAIN_FILE})
endif(CMAKE_TOOLCHAIN_FILE)

# glm
find_package(GLM REQUIRED)

# glslang
set(ENABLE_GLSLANG_BINARIES OFF)
set(ENABLE_SPVREMAPPER OFF)
add_subdirectory(libs/glslang)

# create executable
add_executable(ShaderDebugger ${SOURCES})

# properties
set_target_properties(ShaderDebugger PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# include directories
target_include_directories(ShaderDebugger PRIVATE ${GLM_INCLUDE_DIRS} inc libs libs/glslang)

# link libraries
target_link_libraries(ShaderDebugger ${GLM_LIBRARY_DIRS} glslang)

if (NOT MSVC)
	target_compile_options(ShaderDebugger PRIVATE -Wno-narrowing)
endif()
