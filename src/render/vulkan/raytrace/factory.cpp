
#include "factory.hpp"

#include <shared/math.hpp>

#include "render/vulkan/command/recorder.hpp"
#include "render/vulkan/buffer/query.hpp"
#include "render/api/model.hpp"
#include "render/system.hpp"
#include "shared/logger.hpp"

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
		out::info("Reallocated scratch buffer, now using %d bytes of device memory", bytes);
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
		out::info("Reallocated compaction query pool, now using %d slots", size);
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

	std::vector<VkAccelerationStructureKHR> structures;
	structures.reserve(elements.size());

	std::vector<AccelStructBakedConfig*> linkages;
	linkages.reserve(elements.size());

	int alignment = RenderSystem::system->physical->getScratchBufferAlignment();

	// prepare scratch buffer
	reserveScratchSpace(allocator, batch_scratch + alignment);
	reserveQueryPool(device, elements.size());
	VkDeviceAddress address = math::alignUp(device.getAddress(scratch), alignment);

	// prepare all acceleration structures for building
	for (auto& baked : elements) {
		baked.setScratch(address);
		recorder.buildAccelerationStructure(baked);

		if (baked.build_info.flags & VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR) {
			structures.push_back(baked.model->getStructure().getHandle());
			linkages.push_back(&baked);
		}

		recorder.memoryBarrier()
			.first(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_MEMORY_WRITE_BIT)
			.then(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR)
			.done();
	}

	if (!structures.empty()) {
		recorder.resetQueryPool(query);
		recorder.queryAccelStructProperties(query, structures, VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR);

		std::vector<uint64_t> results;
		results.resize(structures.size());

		// :cursed:
		Fence fence = RenderSystem::system->createFence();
		recorder.quickFenceSubmit(fence, RenderSystem::system->queue);
		fence.reset();

		query.loadAll(results);

		std::vector<AccelStruct> previouses;
		previouses.reserve(structures.size());

		for (size_t i = 0; i < structures.size(); i ++) {

			const uint64_t size = results.at(i);
			AccelStructBakedConfig* config = linkages.at(i);
			std::string new_name = "Compacted " + config->name;

			auto previous = config->model->getStructure();
			previouses.push_back(previous);
			AccelStruct compacted = allocator.allocateAcceleration(config->build_info.type, size, new_name.c_str());
			recorder.copyAccelerationStructure(compacted, previous, true);

			config->model->setStructure(device, compacted);
		}

		recorder.quickFenceSubmit(fence, RenderSystem::system->queue);
		fence.close();

		// close all previous structures
		for (auto& previous : previouses) {
			previous.close(device.getHandle());
		}
	}

	elements.clear();
}
