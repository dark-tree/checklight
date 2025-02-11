
#include "factory.hpp"
#include "render/vulkan/command/recorder.hpp"

void TraceStructFactory::reserveScratchSpace(Allocator& allocator, uint32_t bytes) {

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

std::vector<TraceStruct> TraceStructFactory::bake(const LogicalDevice& device, Allocator& allocator, CommandRecorder& recorder, const std::vector<TraceStructConfig>& configs) {

	std::vector<TraceStructBakedConfig> elements;
	size_t batch_max_scratch = 0;
	elements.reserve(configs.size());

	for (const TraceStructConfig& config : configs) {
		TraceStructBakedConfig baked = config.bake(device);
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
	for (TraceStructBakedConfig& baked : elements) {
		std::string name = "Unnamed TraceStruct";
		TraceStruct structure = allocator.allocateAcceleration(baked.build_info.type, baked.size_info.accelerationStructureSize, name.c_str());

		baked.build_info.scratchData.deviceAddress = address;
		baked.build_info.dstAccelerationStructure = structure.getHandle();
	}

	UNREACHABLE;
}
