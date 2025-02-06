
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
	closeBuffer();
	closeStaging();
}

void ReusableBuffer::closeBuffer() {
	if (buffer) {
		buffer->close();
		buffer.reset();
	}

	count = 0;
}

void ReusableBuffer::closeStaging() {
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
		throw std::runtime_error {"Usage not specified during construction!"};
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
	size_t bytes = elements * size;
	std::memcpy(static_cast<char*>(stage) + offset * size, data, bytes);
}

void ReusableBuffer::flushStaging(RenderCommander& commander) {
	staging->getAllocation().flushNonCoherent();
	commander.getRecorder().copyBufferToBuffer(*buffer, *staging, staging->size(), 0, 0);

}

void ReusableBuffer::resize(RenderCommander& commander, int elements, int size) {

	// if the buffers have not yet been used just allocate
	if (!this->staging) {
		close();
		allocateBuffers(elements, size);
		return;
	}

	auto staging_ptr = this->staging;
	closeBuffer();

	this->count = elements;
	size_t bytes = elements * size;
	std::string staging_name = debug_name + std::string (" Staging");

	buffer = std::make_shared<Buffer>(RenderSystem::system->allocator.allocateBuffer(Memory::DEVICE, bytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, debug_name.c_str()));
	staging = std::make_shared<Buffer>(RenderSystem::system->allocator.allocateBuffer(Memory::STAGED, bytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging_name.c_str()));

	commander.getRecorder()
		.copyBufferToBuffer(*buffer, *staging_ptr, staging_ptr->size(), 0, 0)
		.copyBufferToBuffer(*staging, *staging_ptr, staging_ptr->size(), 0, 0);

	if (staging_ptr) {
		staging_ptr->getAllocation().unmap();
		commander.getTaskQueue().enqueue([staging_ptr] () mutable {
			staging_ptr->close();
		});
	}
}

void ReusableBuffer::upload(RenderCommander& commander, const void* data, int elements, int size) {
	allocateBuffers(elements, size);
	writeToStaging(data, elements, size);
	flushStaging(commander);

	commander.getTaskQueue().enqueue([this] () mutable {
		closeStaging();
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