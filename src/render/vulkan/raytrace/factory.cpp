
#include "factory.hpp"
#include "render/vulkan/command/recorder.hpp"

/*
 * AccelStructFactory
 */

void AccelStructFactory::close() {
	scratch.close();
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

AccelStruct AccelStructFactory::submit(const LogicalDevice& device, Allocator& allocator, AccelStructConfig& config) {
	std::lock_guard lock (mutex);

	AccelStructBakedConfig baked = config.bake(device, allocator);
	uint32_t required = baked.getScratchSize();
	AccelStruct structure = baked.structure;

	elements.emplace_back(std::move(baked));

	if (required > batch_scratch) {
		batch_scratch = required;
	}

	return structure;
}

void AccelStructFactory::bake(const LogicalDevice& device, Allocator& allocator, CommandRecorder& recorder) {
	std::lock_guard lock (mutex);

	// prepare scratch buffer
	reserveScratchSpace(allocator, batch_scratch);
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
