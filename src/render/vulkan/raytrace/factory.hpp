#pragma once

#include "external.hpp"
#include "config.hpp"

class CommandRecorder;

class AccelStructFactory {

	private:

		std::mutex mutex;
		size_t batch_scratch;
		Buffer scratch;
		std::vector<AccelStructBakedConfig> elements;

		void reserveScratchSpace(Allocator& allocator, uint32_t bytes);

	public:

		AccelStructFactory() = default;
		void close();

		AccelStruct submit(const LogicalDevice& device, Allocator& allocator, AccelStructConfig& config);
		void bake(const LogicalDevice& device, Allocator& allocator, CommandRecorder& recorder);

};
