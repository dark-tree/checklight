
#include "instance.hpp"
#include "shared/math.hpp"
#include "render/api/object.hpp"

/*
 * RasterInstanceManager
 */

void RasterInstanceManager::write(const RenderObject& delegate) {
	instance_buffer.writeToStaging(delegate.getInstanceData(), 1, sizeof(VkAccelerationStructureInstanceKHR), delegate.getIndex());
	attachment_buffer.writeToStaging(delegate.getObjectData(), 1, sizeof(RenderObjectData), delegate.getIndex());
}

RasterInstanceManager::RasterInstanceManager()
	: InstanceManager(
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
	){};

std::shared_ptr<RenderObject> RasterInstanceManager::create() {

	// maybe not the best idea, but we can change it later
	if (freed == 0) {
		trim();
	}

	// no blocks to reuse
	if (freed == 0) {
		size_t offset = delegates.size();

		// no space in buffer left
		if (offset >= capacity) {

			capacity = (capacity > 0) ? (capacity * 2) : 16;

			instance_buffer.close();
			instance_buffer.allocateBuffers(capacity, sizeof(VkAccelerationStructureInstanceKHR));

			attachment_buffer.close();
			attachment_buffer.allocateBuffers(capacity, sizeof(RenderObjectData));
		}

		return delegates.emplace_back(std::make_shared<RenderObject>(offset));
	}

	freed --;
	const auto it = std::find(delegates.begin(), delegates.end(), nullptr);
	const size_t offset = std::distance(delegates.begin(), it);

	auto delegate = std::make_shared<RenderObject>(offset);
	(*it) = delegate;

	return delegate;
}