#pragma once

#include "external.hpp"
#include "config.hpp"
#include "render/vulkan/buffer/query.hpp"

class CommandRecorder;

class AccelStructFactory {

	private:

		std::mutex mutex;
		size_t batch_scratch;
		Buffer scratch;
		std::vector<AccelStructBakedConfig> elements;
		QueryPool query;

		void reserveScratchSpace(Allocator& allocator, uint32_t bytes);
		void reserveQueryPool(const LogicalDevice& device, int size);

	public:

		AccelStructFactory() = default;
		void close();

		std::shared_ptr<RenderModel> submit(const LogicalDevice& device, Allocator& allocator, AccelStructConfig& config);
		void bake(const LogicalDevice& device, Allocator& allocator, CommandRecorder& recorder);

};
