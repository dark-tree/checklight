#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/buffer.hpp"

class CommandRecorder;
class RenderCommander;

/**
 * Represents the buffers that underlie RenderMeshes,
 * this structure is part of the public API only on paper as there
 * is no need to ever use it manually outside of the render system.
 */
class ReusableBuffer {

	private:

		std::string debug_name;

		VkBufferUsageFlags usage = 0;
		size_t count = 0;
		std::shared_ptr<Buffer> buffer;

		void* stage = nullptr;
		std::shared_ptr<Buffer> staging;
		VkDeviceAddress address;

	public:

		ReusableBuffer() = default;
		ReusableBuffer(VkBufferUsageFlags flags);

		/// Set resource name
		void setDebugName(const std::string& name);

		/// Close staging and device buffers
		void close();

		/// Close the buffer located in device memory
		void closeDeviceBuffer();

		/// Close the mappable buffer located in host memory
		void closeStagingBuffer();

		/// Create staging and device buffers
		void allocateBuffers(size_t elements, size_t size);

		/// Write data to the staging buffer
		void writeToStaging(const void* data, size_t elements, size_t size, size_t offset = 0);

		/// Copy staging buffer to the device buffer
		void flushStaging(CommandRecorder& recorder);

		/// Change the size of the buffers, all data is discarded
		void resize(int elements, int size);

		/// Allocate buffers, upload data, and free staging buffers
		void upload(RenderCommander& commander, const void* data, int elements, int size);

		/// Check if the buffer is empty
		bool isEmpty() const;

		/// Get reference to the device buffer object
		Buffer& getBuffer() const;

		/// Get element count
		size_t getCount() const;

		/// Get the cached address of device buffer
		VkDeviceAddress getDeviceAddress() const;

};
