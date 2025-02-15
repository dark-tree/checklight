#pragma once

#include "external.hpp"
#include "config.hpp"
#include "render/vulkan/buffer/query.hpp"

class CommandRecorder;

class AccelStructFactory {

	private:

		std::mutex mutex;
		size_t batch_scratch = 0;
		Buffer scratch;
		std::vector<AccelStructBakedConfig> elements;
		QueryPool query;

		void reserveScratchSpace(Allocator& allocator, uint32_t bytes);
		void reserveQueryPool(const LogicalDevice& device, int size);

	public:

		AccelStructFactory() = default;

		/// Close all resources used by the factory
		void close();

		/**
		 * Create new RenderModel (AccelStruct) from AccelStructConfig, the returned model CAN NOT
		 * BE USED until the next call to AccelStructFactory::bake(), setting a unbaked model as a model of
		 * a active (or unactive) RenderObject is invalid and wan't be detected.
		 * @param device
		 * @param allocator
		 * @param config
		 * @return
		 */
		std::shared_ptr<RenderModel> submit(const LogicalDevice& device, Allocator& allocator, AccelStructConfig& config);
		void bake(const LogicalDevice& device, Allocator& allocator, CommandRecorder& recorder);

};
