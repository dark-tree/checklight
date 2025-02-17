
#include "instance.hpp"
#include "shared/math.hpp"
#include "render/api/commander.hpp"
#include "render/api/model.hpp"
#include "render/api/object.hpp"

/*
 * InstanceManager
 */

void InstanceManager::write(const RenderObject& delegate) {
	buffer.writeToStaging(delegate.getInstanceData(), 1, sizeof(VkAccelerationStructureInstanceKHR), delegate.getIndex());
	object_data_buffer.writeToStaging(&delegate.getObjectData(), 1, sizeof(RenderObjectData), delegate.getIndex());
}

void InstanceManager::trim() {
	for (std::shared_ptr<RenderObject>& delegate : delegates) {

		// get rid of unused references
		// https://en.cppreference.com/w/cpp/memory/shared_ptr/use_count#Notes
		if (delegate.use_count() <= 1) {
			delegate.reset();
			freed ++;
		}
	}
}

InstanceManager::InstanceManager()
: buffer(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR),
  object_data_buffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
	capacity = 0;
	buffer.setDebugName("Instance Array");
	object_data_buffer.setDebugName("Instance Data");
}

InstanceManager::~InstanceManager() {
	buffer.close();
	object_data_buffer.close();
}

std::shared_ptr<RenderObject> InstanceManager::create() {

	// maybe not the best idea, but we can change it later
	if (freed == 0) {
		trim();
	}

	// no blocks to reuse
	if (freed == 0) {
		size_t offset = delegates.size();

		// no space in buffer left
		if (offset >= capacity) {
			// TODO
			capacity = (capacity > 0) ? (capacity * 2) : 1;

			buffer.close();
			buffer.allocateBuffers(capacity, sizeof(VkAccelerationStructureInstanceKHR));

			object_data_buffer.close();
			object_data_buffer.allocateBuffers(capacity, sizeof(RenderObjectData));
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

void InstanceManager::flush(CommandRecorder& recorder) {
	for (const std::shared_ptr<RenderObject>& delegate : delegates) {
		write(*delegate);
	}

	buffer.flushStaging(recorder);
	object_data_buffer.flushStaging(recorder);
}

const ReusableBuffer& InstanceManager::getBuffer() const {
	return buffer;
}

const ReusableBuffer& InstanceManager::getObjectDataBuffer() const {
	return object_data_buffer;
}
