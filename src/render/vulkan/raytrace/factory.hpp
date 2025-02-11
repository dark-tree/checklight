#pragma once

#include "external.hpp"
#include "config.hpp"

class TraceStructFactory {

	public:

		std::vector<TraceStruct> bake(const std::vector<TraceStructConfig>& configs);

};
