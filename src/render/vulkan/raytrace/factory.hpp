#pragma once

#include "external.hpp"
#include "config.hpp"

class CommandRecorder;

class TraceStructFactory {

	private:

		Buffer scratch;

		void reserveScratchSpace(Allocator& allocator, uint32_t bytes);

	public:

		std::vector<TraceStruct> bake(const LogicalDevice& device, Allocator& allocator, CommandRecorder& recorder, const std::vector<TraceStructConfig>& configs);

};
