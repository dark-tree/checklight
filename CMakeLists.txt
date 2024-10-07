cmake_minimum_required(VERSION 3.20)

project(CHECKLIGHT)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(FETCHCONTENT_QUIET OFF)

find_package(Python REQUIRED COMPONENTS Interpreter)
find_package(Threads REQUIRED)
find_package(Vulkan REQUIRED)

# Try: cmake . -B build && cmake --build build/
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
	message(FATAL_ERROR "In source building is deprecated, select another build directory!")
endif()

# Use the mold linker automatically for GCC if available
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	find_program(MOLD_PROGRAM mold)

	if (MOLD_PROGRAM)
		message(STATUS "The MOLD linker was found: ${MOLD_PROGRAM}")
		set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=mold")
		set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=mold")
	else()
		message(STATUS "The MOLD linker was NOT found, consider installing it!")
	endif()
endif()

FetchContent_Declare(
		glfw
		GIT_REPOSITORY https://github.com/glfw/glfw.git
		GIT_TAG 3.4
		GIT_PROGRESS TRUE
)

FetchContent_Declare(
		stb
		GIT_REPOSITORY https://github.com/nothings/stb
		GIT_TAG 5736b15f7ea0ffb08dd38af21067c314d6a3aae9
		GIT_PROGRESS TRUE
)

FetchContent_Declare(
		glm
		GIT_REPOSITORY https://github.com/g-truc/glm
		GIT_TAG 0.9.9.8
		GIT_PROGRESS TRUE
)

FetchContent_Declare(
		shaderc
		GIT_REPOSITORY https://github.com/google/shaderc
		GIT_TAG v2023.4
		GIT_PROGRESS TRUE
)

# Time for some mental gymnastics for shaderc ...
FetchContent_Populate(shaderc)
message(STATUS "Fetching external dependencies for target 'shaderc'...")

# Invoke their very special dependency python script
execute_process(
		COMMAND ${Python_EXECUTABLE} -u ${shaderc_SOURCE_DIR}/utils/git-sync-deps
		COMMAND_ECHO STDOUT
		WORKING_DIRECTORY ${shaderc_SOURCE_DIR}
		RESULT_VARIABLE GIT_SYNC_DEPS_RESULT
)

if(NOT GIT_SYNC_DEPS_RESULT EQUAL 0)
	message(FATAL_ERROR "Failed to sync Shaderc dependencies")
endif()

# Set Shaderc specific options
set(SKIP_SPIRV_TOOLS_INSTALL ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_INSTALL ON CACHE BOOL "" FORCE)

# ... And finally add shaderc!
add_subdirectory(${shaderc_SOURCE_DIR} ${shaderc_BINARY_DIR})
FetchContent_MakeAvailable(glfw stb glm)

include_directories(
		src/
		${glfw_SOURCE_DIR}/include               # Headers in ./GLFW
		${stb_SOURCE_DIR}                        # Headers in ./
		${glm_SOURCE_DIR}                        # Headers in ./glm
		${shaderc_SOURCE_DIR}/libshaderc/include # Headers in ./shaderc
		${Vulkan_INCLUDE_DIRS}                   # Headers in ./vulcan
)

file(GLOB_RECURSE CHECKLIGHT_SOURCES
		"src/*.cpp"
)

# basic unit test are build as a separate target
list(FILTER VLT3D_SOURCES EXCLUDE REGEX "src[\\/]test\\.cpp$")
list(FILTER VLT3D_SOURCES EXCLUDE REGEX "src[\\/]main\\.cpp$")

add_library(external
		lib/stb_image.cpp
		lib/stb_image_write.cpp
		lib/stb_vorbis.cpp
)

add_executable(main
		"src/main.cpp"
		${CHECKLIGHT_SOURCES}
)

set(CHECKLIGHT_LIBS
		Threads::Threads
		Vulkan::Vulkan
		glfw
		shaderc
		external
)

target_link_libraries(main PRIVATE ${CHECKLIGHT_LIBS})
target_compile_definitions(main PRIVATE "SOURCE_ROOT=\"${CMAKE_SOURCE_DIR}\"")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

	# GCC debugging doodads
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		message(STATUS "Compiling with debugging symbols")
		target_compile_options(main PRIVATE -g)
	endif()

	set(CHECKLIGHT_WARNING_FLAGS
			-Wno-volatile
			-Wall
			-Werror=return-type
			-Werror=return-local-addr
	)

	target_compile_options(external PRIVATE -Wno-volatile)
	target_compile_options(main PRIVATE ${CHECKLIGHT_WARNING_FLAGS})
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

	# we4715 - treat missing return statement as error
	target_compile_options(main PRIVATE /W4 /we4715)
endif()

# Copy assets if any exist
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
	# This seams to work or at least be better but if there are still issues
	# consider doing it like this: https://stackoverflow.com/a/77075253

	add_custom_target(copy-assets ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_SOURCE_DIR}/assets"
			"${CMAKE_BINARY_DIR}/assets"
	)

	add_dependencies(main copy-assets)
endif()
