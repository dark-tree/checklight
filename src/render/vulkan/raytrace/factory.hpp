#pragma once

#include "external.hpp"
#include "config.hpp"

class CommandRecorder;

class AccelStructFactory {

	private:

		Buffer scratch;

		void reserveScratchSpace(Allocator& allocator, uint32_t bytes);

	public:

		AccelStructFactory() = default;
		void close();

		std::vector<AccelStruct> bake(const LogicalDevice& device, Allocator& allocator, CommandRecorder& recorder, const std::vector<AccelStructConfig>& configs);

};
