
#include "const.hpp"

// C libs
#define _USE_MATH_DEFINES
#include <cmath>

// C++ libs
#include <iostream>
#include <vector>
#include <functional>
#include <list>
#include <algorithm>
#include <concepts>
#include <fstream>
#include <sstream>
#include <queue>
#include <mutex>
#include <set>
#include <map>
#include <memory>
#include <filesystem>
#include <utility>

// GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// STB
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"
#include "stb_image.h"
#include "stb_image_write.h"

// VMA
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VMA_VULKAN_VERSION 1000000
#include "vk_mem_alloc.h"

// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/norm.hpp>

// Vulkan Format Database
#include "vk_format_info.hpp"

/**
 * Can by use in function bodies to mark a spot that should normally be unreachable
 * during execution, for example after an enum switch. Sometimes needed when
 * the '-Werror=return-type' flag is used as the compiler is not always smart enough
 */
#ifdef __GNUC__ // GCC 4.8+, Clang, Intel and other compilers compatible with GCC (-std=c++0x or above)
#	define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER) // MSVC
#	define UNREACHABLE __assume(false)
#else
#	define UNREACHABLE std::terminate()
#endif

/**
 * Can be used to mark pointer function parameters that can be set to nullptr
 */
#define NULLABLE
