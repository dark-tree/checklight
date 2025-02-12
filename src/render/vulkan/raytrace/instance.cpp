
#include "instance.hpp"
#include "shared/math.hpp"
#include "render/api/commander.hpp"

/*
 * RenderObject
 */

RenderObject::RenderObject(uint32_t index)
: index(index), instance({}) {
	instance.instanceCustomIndex = index;
	instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
}

const VkAccelerationStructureInstanceKHR* RenderObject::getData() const {
	return &instance;
}

uint32_t RenderObject::getIndex() const {
	return index;
}

void RenderObject::setMatrix(const glm::mat4x3& model) {
	instance.transform = math::toVulkanAffine(model);
}

void RenderObject::setShader(uint32_t offset) {
	instance.instanceShaderBindingTableRecordOffset = offset;
}

void RenderObject::setTraits(VkGeometryInstanceFlagsKHR flags) {
	instance.flags = flags;
}

void RenderObject::setObject() {
	// TODO set BLAS address in instance.accelerationStructureReference
}

/*
 * InstanceManager
 */

void InstanceManager::write(const RenderObject& delegate) {
	buffer.writeToStaging(delegate.getData(), 1, sizeof(VkAccelerationStructureInstanceKHR), delegate.getIndex());
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
: buffer(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR) {
	capacity = 0;
	buffer.setDebugName("Instance Array");
}

InstanceManager::~InstanceManager() {
	buffer.close();
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
			capacity = (capacity > 0) ? (capacity * 2) : 128;

			buffer.close();
			buffer.allocateBuffers(capacity, sizeof(VkAccelerationStructureInstanceKHR));
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

void InstanceManager::flush(RenderCommander& commander) {
	for (const std::shared_ptr<RenderObject>& delegate : delegates) {
		write(*delegate);
	}

	buffer.flushStaging(commander);
}

const ReusableBuffer& InstanceManager::getBuffer() const {
	return buffer;
}