
#include "factory.hpp"
#include "render/vulkan/command/recorder.hpp"
#include "render/vulkan/buffer/query.hpp"
#include "render/api/model.hpp"
#include "render/system.hpp"

/*
 * AccelStructFactory
 */

void AccelStructFactory::close() {
	scratch.close();
	query.close();
}

void AccelStructFactory::reserveScratchSpace(Allocator& allocator, uint32_t bytes) {

	// make sure we allocate some buffer even if not asked to
	if (bytes < 16) {
		bytes = 16;
	}

	if (scratch.size() < bytes) {
		scratch.close();
		scratch = allocator.allocateBuffer(Memory::DEVICE, bytes, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, "Scratch");
		printf("INFO: Reallocated scratch buffer, now using %d bytes of device memory\n", bytes);
	}
}

void AccelStructFactory::reserveQueryPool(const LogicalDevice& device, int size) {

	// make sure we allocate some query pool even if not asked to
	if (size < 16) {
		size = 16;
	}

	if (static_cast<int>(query.size()) < size) {
		query.close();

		query = QueryPool {device, VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR, size};
		query.setDebugName("Compaction Query Pool");
		printf("INFO: Reallocated compaction query pool, now using %d slots\n", size);
	}
}

std::shared_ptr<RenderModel> AccelStructFactory::submit(const LogicalDevice& device, Allocator& allocator, AccelStructConfig& config) {
	std::lock_guard lock (mutex);

	AccelStructBakedConfig baked = config.bake(device, allocator);
	uint32_t required = baked.getScratchSize();
	auto model = baked.model;

	elements.emplace_back(std::move(baked));

	if (required > batch_scratch) {
		batch_scratch = required;
	}

	return model;
}

void AccelStructFactory::bake(const LogicalDevice& device, Allocator& allocator, CommandRecorder& recorder) {
	std::lock_guard lock (mutex);

	// prepare scratch buffer
	reserveScratchSpace(allocator, batch_scratch);
	reserveQueryPool(device, elements.size());
	VkDeviceAddress address = device.getAddress(scratch);

	// prepare all acceleration structures for building
	for (auto& baked : elements) {
		baked.setScratch(address);
		recorder.buildAccelerationStructure(baked);

		recorder.memoryBarrier()
			.first(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_MEMORY_WRITE_BIT)
			.then(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR)
			.done();
	}

	elements.clear();
}
