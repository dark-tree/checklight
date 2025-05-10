
#include "reusable.hpp"
#include "render/system.hpp"
#include "render/api/commander.hpp"

/*
 * ReusableBuffer
 */

ReusableBuffer::ReusableBuffer(VkBufferUsageFlags usage)
: usage(usage) {}

void ReusableBuffer::setDebugName(const std::string& name) {
	this->debug_name = name;
}

void ReusableBuffer::close() {
	closeDeviceBuffer();
	closeStagingBuffer();
}

void ReusableBuffer::closeDeviceBuffer() {
	if (buffer) {
		buffer->close();
		buffer.reset();
	}

	count = 0;
}

void ReusableBuffer::closeStagingBuffer() {
	if (staging) {
		if (staging->allocated()) {
			staging->getAllocation().unmap();
			staging->close();
		}

		staging.reset();
	}
}

void ReusableBuffer::allocateBuffers(size_t elements, size_t size) {
	if (usage == 0) {
		FAULT("Usage not specified during construction!");
	}

	size_t bytes = elements * size;
	std::string staging_name = debug_name + std::string (" Staging");

	close();
	buffer = std::make_shared<Buffer>(RenderSystem::system->allocator.allocateBuffer(Memory::DEVICE, bytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, debug_name.c_str()));
	staging = std::make_shared<Buffer>(RenderSystem::system->allocator.allocateBuffer(Memory::STAGED, bytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging_name.c_str()));
	stage = staging->getAllocation().map();
	count = elements;
}

void ReusableBuffer::writeToStaging(const void* data, size_t elements, size_t size, size_t offset) {
	if (!this->staging) {
		FAULT("Can't write to staging, buffer missing!");
	}

	size_t bytes = elements * size;
	std::memcpy(static_cast<char*>(stage) + offset * size, data, bytes);
}

void ReusableBuffer::flushStaging(CommandRecorder& recorder) {
	if (this->staging) {
		staging->getAllocation().flushNonCoherent();
		recorder.copyBufferToBuffer(*buffer, *staging, staging->size(), 0, 0);
	}
}

void ReusableBuffer::resize(int elements, int size) {

	const size_t bytes = elements * size;

	if (!this->staging || !this->buffer) {
		allocateBuffers(elements, size);
		return;
	}

	if (buffer->size() < bytes) {
		allocateBuffers(elements, size);
	}

}

void ReusableBuffer::upload(RenderCommander& commander, const void* data, int elements, int size) {
	allocateBuffers(elements, size);
	writeToStaging(data, elements, size);
	flushStaging(commander.getRecorder());

	commander.getTaskQueue().enqueue([this] () mutable {
		closeStagingBuffer();
	});
}

bool ReusableBuffer::isEmpty() const {
	return getCount() == 0;
}

Buffer& ReusableBuffer::getBuffer() const {
	return *buffer;
}

size_t ReusableBuffer::getCount() const {
	return count;
}

VkDeviceAddress ReusableBuffer::getDeviceAddress() const {
	return RenderSystem::system->device.getAddress(*buffer);
}