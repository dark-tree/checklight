#pragma once

#include "external.hpp"

/// Check if the type T is a number
template <typename T>
concept Numerical = std::is_floating_point_v<T> || std::is_integral_v<T>;