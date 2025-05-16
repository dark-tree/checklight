#pragma once

#define ENGINE_VERSION VK_MAKE_VERSION(1, 0, 0)
#define ENGINE_NAME "Checklight"

/*
 * If ENGINE_DEBUG true engine performs extra checks, and generate extra fields unnecessary for working correctly,
 * but to ensure its working as intended. define NDEBUG for release build
 */
#ifndef NDEBUG
#	define ENGINE_DEBUG true
#else
#	define ENGINE_DEBUG false
#endif

#define TICK_DURATION 0.02