
#include "instance.hpp"
#include "render/api/commander.hpp"

/*
 * InstanceDelegate
 */

InstanceDelegate::InstanceDelegate(size_t index)
: instance({}), index(index) {
	instance.instanceCustomIndex = index;
	instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
}

void InstanceDelegate::setMatrix(const glm::mat3x4& model) {
	VkTransformMatrixKHR& transform = instance.transform;

	for (int row = 0; row < 3; row ++) {
		for (int column = 0; column < 4; column ++) {
			transform.matrix[row][column] = model[column][row];
		}
	}
}

void InstanceDelegate::setShader(uint32_t offset) {
	instance.instanceShaderBindingTableRecordOffset = offset;
}

void InstanceDelegate::setTraits(VkGeometryInstanceFlagsKHR flags) {
	instance.flags = flags;
}

void InstanceDelegate::setObject() {
	// TODO set BLAS address in instance.accelerationStructureReference
}

/*
 * InstanceManager
 */

void InstanceManager::write(const InstanceDelegate& delegate) {
	buffer.writeToStaging(&delegate.instance, 1, sizeof(VkAccelerationStructureInstanceKHR), delegate.index);
}

void InstanceManager::trim() {
	for (std::shared_ptr<InstanceDelegate>& delegate : delegates) {

		// get rid of unused references
		if (delegate.unique()) {
			delegate.reset();
			freed ++;
		}
	}
}

InstanceManager::InstanceManager()
: buffer(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR) {
	capacity = 0;
	buffer.setDebugName("Instance Array");
}

InstanceManager::~InstanceManager() {
	buffer.close();
}

std::shared_ptr<InstanceDelegate> InstanceManager::create() {

	// maybe not the best idea, but we can change it later
	if (freed == 0) {
		trim();
	}

	// no blocks to reuse
	if (freed == 0) {
		size_t offset = delegates.size();

		// no space in buffer left
		if (offset >= capacity) {
			capacity = (capacity > 0) ? (capacity * 2) : 128;

			buffer.close();
			buffer.allocateBuffers(capacity, sizeof(VkAccelerationStructureInstanceKHR));
		}

		return delegates.emplace_back(std::make_shared<InstanceDelegate>(offset));
	}

	freed --;
	const auto it = std::find(delegates.begin(), delegates.end(), nullptr);
	const size_t offset = std::distance(delegates.begin(), it);

	auto delegate = std::make_shared<InstanceDelegate>(offset);
	(*it) = delegate;

	return delegate;
}

void InstanceManager::flush(RenderCommander& commander) {
	for (const std::shared_ptr<InstanceDelegate>& delegate : delegates) {
		write(*delegate);
	}

	buffer.flushStaging(commander);
}

Buffer& InstanceManager::getBuffer() {
	return buffer.getBuffer();
}