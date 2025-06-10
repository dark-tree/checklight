#pragma once
#include "external.hpp"

//S - base, T - child
template<typename T, typename S>
concept DerivedTrait = std::is_base_of_v<S, T>;
