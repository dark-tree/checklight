
#include "instance.hpp"
#include "shared/math.hpp"
#include "render/api/model.hpp"
#include "render/api/object.hpp"

/*
 * InstanceManager
 */


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

InstanceManager::InstanceManager(const VkBufferUsageFlags instance_flags, const VkBufferUsageFlags attachment_flags)
: instance_buffer(instance_flags),
  attachment_buffer(attachment_flags) {
	capacity = 0;
	instance_buffer.setDebugName("Instance Array");
	attachment_buffer.setDebugName("Attachment Array");
}

InstanceManager::~InstanceManager() {
	instance_buffer.close();
	attachment_buffer.close();
}


void InstanceManager::flush(CommandRecorder& recorder) {
	for (const std::shared_ptr<RenderObject>& delegate : delegates) {
		write(*delegate);
	}

	instance_buffer.flushStaging(recorder);
	attachment_buffer.flushStaging(recorder);
}

const ReusableBuffer& InstanceManager::getInstanceBuffer() const {
	return instance_buffer;
}

const ReusableBuffer& InstanceManager::getAttachmentBuffer() const {
	return attachment_buffer;
}

uint32_t InstanceManager::count() const {
	return static_cast<uint32_t>(delegates.size());
}

std::vector<std::shared_ptr<RenderObject>> InstanceManager::getDelegates() const {
	return delegates;
}
