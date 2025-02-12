#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/buffer.hpp"

class CommandRecorder;
class RenderCommander;

class ReusableBuffer {

	private:

		std::string debug_name;

		VkBufferUsageFlags usage = 0;
		size_t count = 0;
		std::shared_ptr<Buffer> buffer;

		void* stage = nullptr;
		std::shared_ptr<Buffer> staging;

	public:

		ReusableBuffer() = default;
		ReusableBuffer(VkBufferUsageFlags flags);

		void setDebugName(const std::string& name);

		void close();
		void closeBuffer();
		void closeStaging();

		void allocateBuffers(size_t elements, size_t size);
		void writeToStaging(const void* data, size_t elements, size_t size, size_t offset = 0);
		void flushStaging(CommandRecorder& recorder);

		void resize(RenderCommander& commander, int elements, int size);
		void upload(RenderCommander& commander, const void* data, int elements, int size);

		bool isEmpty() const;
		Buffer& getBuffer() const;
		size_t getCount() const;

};
