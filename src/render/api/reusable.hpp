#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/buffer.hpp"

class RenderCommander;

class ReusableBuffer {

	private:

		size_t count = 0;
		Buffer buffer;

	public:

		ReusableBuffer() = default;

		void close();

		void upload(RenderCommander& commander, const void* data, int elements, int size, VkBufferUsageFlags flags);

		bool isEmpty() const;
		Buffer getBuffer() const;
		size_t getCount() const;

};
