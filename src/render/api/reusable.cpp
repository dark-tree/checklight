
#include "reusable.hpp"
#include "render/system.hpp"
#include "render/api/commander.hpp"

void ReusableBuffer::close() {
	buffer.close();
	count = 0;
}

void ReusableBuffer::upload(RenderCommander& commander, const void* data, int elements, int size, VkBufferUsageFlags flags) {
	buffer.close();
	count = elements;

	const size_t bytes = elements * size;
	RenderSystem& system = *RenderSystem::system;

	VkBufferUsageFlags staged_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	Buffer staged = system.allocator.allocateBuffer(Memory::STAGED, bytes, staged_usage);

	VkBufferUsageFlags buffer_usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | flags;
	buffer = system.allocator.allocateBuffer(Memory::DEVICE, bytes, buffer_usage);

	// copy to staging
	Allocation& allocation = staged.getAllocation();
	void* memory = allocation.map();
	std::memcpy(memory, data, bytes);
	allocation.unmap();
	allocation.flushNonCoherent();

	// copy to device local
	commander.getRecorder().copyBufferToBuffer(buffer, staged, bytes);
	commander.getTaskQueue().enqueue([staged] () mutable {
		staged.close();
	});
}

bool ReusableBuffer::isEmpty() const {
	return getCount() == 0;
}

Buffer ReusableBuffer::getBuffer() const {
	return buffer;
}

size_t ReusableBuffer::getCount() const {
	return count;
}