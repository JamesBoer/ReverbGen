
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Set project name
project(ReverbGen)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable IDE folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(
    reverb_source_list
	"Source/ReverbGen.cpp"
    "Source/ReverbGen.h"
	"Source/RgHistogram.h"
    "Source/RgInternal.h"
)

# Create library as project name
add_library(${PROJECT_NAME} ${reverb_source_list})

# Set compiler options
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /permissive- /W4 /w45038 /WX)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Create IDE folder structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${reverb_source_list})

# Create IDE folder structure
source_group("Source" FILES ${demo_source_list})

# Test project
set(
	tests_source_list
	"Test/catch.hpp"
	"Test/Main.cpp"
	"Test/TestHistogram.cpp"
	"Test/TestSignatures.cpp"
)

# Create Test executable
add_executable(Test ${tests_source_list})
target_link_libraries(Test ReverbGen)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Test)

# Create IDE folder structure
source_group("Source" FILES ${tests_source_list})
	

