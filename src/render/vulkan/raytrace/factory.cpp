
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
		scratch = allocator.allocateBuffer(Memory::DEVICE, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, bytes, "Scratch");
		printf("INFO: Reallocated scratch buffer, now using %d bytes of device memory\n", bytes);
	}
}

std::vector<AccelStruct> AccelStructFactory::bake(const LogicalDevice& device, Allocator& allocator, CommandRecorder& recorder, const std::vector<AccelStructConfig>& configs) {

	std::vector<AccelStructBakedConfig> elements;
	std::vector<AccelStruct> structures;

	size_t batch_max_scratch = 0;
	elements.reserve(configs.size());
	structures.reserve(configs.size());

	for (const AccelStructConfig& config : configs) {
		AccelStructBakedConfig baked = config.bake(device);
		uint32_t required = baked.getScratchSize();
		elements.emplace_back(std::move(baked));

		if (required > batch_max_scratch) {
			batch_max_scratch = required;
		}
	}

	// prepare scratch buffer
	reserveScratchSpace(allocator, batch_max_scratch);
	VkDeviceAddress address = device.getAddress(scratch);

	// prepare all acceleration structures for building
	for (auto& baked : elements) {
		AccelStruct structure = allocator.allocateAcceleration(baked.build_info.type, baked.size_info.accelerationStructureSize, "Unnamed AccelStruct");
		baked.finalize(structure, address);
		recorder.buildAccelerationStructure(baked);
		structures.emplace_back(structure);

		recorder.memoryBarrier()
			.first(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_MEMORY_WRITE_BIT)
			.then(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR)
			.done();
	}

	return structures;
}
