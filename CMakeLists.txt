cmake_minimum_required(VERSION 3.20)

project(CHECKLIGHT)
include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(FETCHCONTENT_QUIET OFF)

# When one CMAKE File is not enough
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(${CMAKE_SOURCE_DIR}/cmake/ShaderBuild.cmake)

find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)
find_package(Vulkan REQUIRED)
find_package(OpenAL)

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

if (NOT OpenAL_FOUND)
    message(STATUS "OpenAL was NOT found, falling back to OpenAL-Soft!")

    FetchContent_Declare(
            openal-soft
            GIT_REPOSITORY https://github.com/kcat/openal-soft
            GIT_TAG 1.23.1
            GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(openal-soft)
endif()

# Don't fail on systems without X11 libraries
if (UNIX)
	find_program(WAYLAND_SCANNER_EXECUTABLE NAMES wayland-scanner)
	if (NOT WAYLAND_SCANNER_EXECUTABLE)
		message(WARNING
				"Failed to find wayland-scanner! "
				"The final executable will not work on Wayland only systems, "
				"consider installing Wayland development libraries!"
		)

		set(GLFW_BUILD_WAYLAND "0" CACHE STRING "" FORCE)
	endif()
endif()

FetchContent_Declare(
		vstl
		GIT_REPOSITORY https://github.com/magistermaks/lib-vstl
		GIT_TAG v3.2
)

FetchContent_Declare(
		msdfgen
		GIT_REPOSITORY https://github.com/Chlumsky/msdfgen
		GIT_TAG v1.12
		GIT_PROGRESS TRUE
)

FetchContent_Declare(
		vma
		GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
		GIT_TAG v3.2.0
		GIT_PROGRESS TRUE
)

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

set(MSDFGEN_DISABLE_SVG ON CACHE BOOL "" FORCE)
set(MSDFGEN_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(MSDFGEN_USE_SKIA OFF CACHE BOOL "" FORCE)
set(MSDFGEN_USE_VCPKG OFF CACHE BOOL "" FORCE)
set(MSDFGEN_BUILD_STANDALONE OFF CACHE BOOL "" FORCE)
set(MSDFGEN_DYNAMIC_RUNTIME ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw stb glm vma msdfgen vstl)

compile_glsl_directory(
		SRC "${CMAKE_CURRENT_SOURCE_DIR}/shader"
		DST "${CMAKE_CURRENT_BINARY_DIR}/assets/shader"
		VULKAN_TARGET "vulkan1.2"
)

if(NOT DEFINED OpenAL_INCLUDE_DIRS)
	set(OpenAL_INCLUDE_DIRS "${OpenAL_SOURCE_DIR}/include")
	message(STATUS "Applying OpenAL include directory fix")
endif()

set(CHECKLIGHT_LIBRARIES
		${glm_SOURCE_DIR}                        # Headers in ./glm
		${glfw_SOURCE_DIR}/include               # Headers in ./GLFW
		${stb_SOURCE_DIR}                        # Headers in ./
		${Vulkan_INCLUDE_DIRS}                   # Headers in ./vulcan
		${vma_SOURCE_DIR}/include                # Headers in ./
		${OpenAL_INCLUDE_DIRS}                   # Headers in ./AL
)

add_library(checklight_common INTERFACE)
target_include_directories(checklight_common INTERFACE
		src/
		lib/
		${CHECKLIGHT_LIBRARIES}
)

file(GLOB_RECURSE CHECKLIGHT_ENGINE_SOURCES "src/engine/*.cpp")
add_library(checklight_engine_system ${CHECKLIGHT_ENGINE_SOURCES})

file(GLOB_RECURSE CHECKLIGHT_RENDER_SOURCES "src/render/*.cpp")
add_library(checklight_render_system ${CHECKLIGHT_RENDER_SOURCES} )

file(GLOB_RECURSE CHECKLIGHT_SHARED_SOURCES "src/shared/*.cpp")
add_library(checklight_shared_system ${CHECKLIGHT_SHARED_SOURCES} )

file(GLOB_RECURSE CHECKLIGHT_INPUT_SOURCES "src/input/*.cpp")
add_library(checklight_input_system ${CHECKLIGHT_INPUT_SOURCES} )

file(GLOB_RECURSE CHECKLIGHT_SOUND_SOURCES "src/sound/*.cpp")
add_library(checklight_sound_system ${CHECKLIGHT_SOUND_SOURCES} )

file(GLOB_RECURSE CHECKLIGHT_GUI_SOURCES "src/gui/*.cpp")
add_library(checklight_gui_system ${CHECKLIGHT_GUI_SOURCES})

add_library(checklight_external
		lib/stb_image.cpp
		lib/stb_image_write.cpp
		lib/stb_vorbis.cpp
		lib/vk_mem_alloc.cpp
 )

target_include_directories(checklight_external PRIVATE
		${CHECKLIGHT_LIBRARIES}
)

add_executable(checklight "src/main.cpp")
add_executable(checklight_test "src/test.cpp")

target_link_libraries(checklight_engine_system PRIVATE
		checklight_common
		checklight_shared_system
		checklight_render_system
)

target_link_libraries(checklight_render_system PRIVATE
		checklight_common
		Vulkan::Vulkan
		glfw
		checklight_external
		msdfgen::msdfgen-core
		msdfgen::msdfgen-ext
		Freetype::Freetype
		checklight_shared_system
)

target_link_libraries(checklight_shared_system PRIVATE
		checklight_common
		Threads::Threads
		checklight_external
)

target_link_libraries(checklight_input_system PRIVATE
		checklight_common
)

target_link_libraries(checklight_sound_system PRIVATE
		checklight_common
		OpenAL::OpenAL
		checklight_external
)

target_link_libraries(checklight_gui_system PRIVATE
		checklight_common
		checklight_input_system
		checklight_render_system
		checklight_shared_system
)

target_link_libraries(checklight PRIVATE
		checklight_common
		checklight_render_system
		checklight_shared_system
		checklight_input_system
		checklight_sound_system
		checklight_gui_system
		checklight_engine_system
)

target_link_libraries(checklight_test PRIVATE
		checklight_common
		checklight_gui_system
		checklight_shared_system
		checklight_render_system
		checklight_engine_system
)

target_include_directories(checklight_test PRIVATE
		${vstl_SOURCE_DIR}/
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

	# GCC debugging doodads
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		message(STATUS "Compiling with debugging symbols")
		target_compile_options(checklight_common INTERFACE -g)
	endif()

	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		message(STATUS "Compiling with striped symbols")
		target_link_options(checklight_common INTERFACE -s -z now)
	endif()

	target_compile_options(checklight_common INTERFACE
			-Wall
			-Wextra
			-pedantic

			# Errors
			-Werror=return-type         # Fail if when return in non-void function is missing
			-Werror=return-local-addr   # Fail if a pointer to a local variable is returned
			-Werror=init-self           # Fail if a class member is initialized with itself
			-Werror=uninitialized       # Fail if a uninitialized variables is used
			-Werror=cast-qual           # Fail when you try to cast the const away

			# Warnings
			-Wnoexcept                  # Warn if the lack of noexcept disables some optimizations
			-Wunreachable-code          # Warn if a section of code is unreachable
			-Wparentheses               # Warn if there is a high chance we forgot about some parentheses
			-Wlogical-op                # Warn if there is a high chance we messed up the logical operator
			-Wdisabled-optimization     # Warn if you managed to confuse the compiler :skull:
			-Wswitch-default            # Warn whenever a switch statement does not have a default case
			-Wredundant-decls           # Warn if anything is declared more than once in the same scope
			-Wmissing-declarations      # Warn if a global function is defined without a previous declaration

			# Ignore some warnings that I deemed stupid
			-Wno-unused-parameter       # Not really an error to ignore something, we may enable it later
			-Wno-volatile               # GLM does this, so unless we want hundreds of warnings we need to disable this
			-Wno-variadic-macros        # Allow so called 'variadic macros'
			-Wno-format-security        # We have to disable it to create our own print-like functions
			-Wno-misleading-indentation # Don't complain about non-issues such as "if (thing) do();"
	)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

	# we4715 - treat missing return statement as error
	target_compile_options(checklight_common INTERFACE /W4 /we4715)
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

	add_custom_target(compile-shaders
			DEPENDS ${SPV_OUTPUT}
	)

	add_dependencies(checklight_common copy-assets)
	add_dependencies(checklight_common compile-shaders)
endif()

# Needed on MSVS to fix missing DLL errors during runtime
if (WIN32)
	add_custom_command(TARGET checklight_sound_system POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${openal-soft_BINARY_DIR}/OpenAL32.dll"
			"${CMAKE_BINARY_DIR}/OpenAL32.dll")
endif()