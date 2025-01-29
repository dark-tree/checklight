#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/system.hpp"
#include "commander.hpp"

class RenderMesh {

	private:

		size_t vertex_count = 0;
		Buffer vertex;

		size_t index_count = 0;
		Buffer index;

	public:

		~RenderMesh();

		template <typename V>
		void uploadVertices(RenderCommander& commander, const std::vector<V>& vertices) {
			vertex.close();
			vertex_count = vertices.size();

			const size_t bytes = vertices.size() * sizeof(V);
			RenderSystem& system = *RenderSystem::system;

			VkBufferUsageFlags staged_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			Buffer staged = system.allocator.allocateBuffer(Memory::STAGED, bytes, staged_usage);

			VkBufferUsageFlags vertex_usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			vertex = system.allocator.allocateBuffer(Memory::DEVICE, bytes, vertex_usage);

			// copy to staging
			Allocation& allocation = staged.getAllocation();
			void* memory = allocation.map();
			std::memcpy(memory, vertices.data(), bytes);
			allocation.unmap();
			allocation.flushNonCoherent();

			// copy to device local
			commander.getRecorder().copyBufferToBuffer(vertex, staged, bytes);
			commander.getTaskQueue().enqueue([staged] () mutable {
				staged.close();
			});

		}

		template <typename I>
		void uploadIndices(RenderCommander& commander, const std::vector<I>& indices) {
			index.close();
			index_count = indices.size();

			const size_t bytes = indices.size() * sizeof(I);
			RenderSystem& system = *RenderSystem::system;

			VkBufferUsageFlags staged_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			Buffer staged = system.allocator.allocateBuffer(Memory::STAGED, bytes, staged_usage);

			VkBufferUsageFlags vertex_usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			index = system.allocator.allocateBuffer(Memory::DEVICE, bytes, vertex_usage);

			// copy to staging
			Allocation& allocation = staged.getAllocation();
			void* memory = allocation.map();
			std::memcpy(memory, indices.data(), bytes);
			allocation.unmap();
			allocation.flushNonCoherent();

			// copy to device local
			commander.getRecorder().copyBufferToBuffer(index, staged, bytes);
			commander.getTaskQueue().enqueue([staged] () mutable {
				staged.close();
			});

		}

		bool hasIndexData() const;
		void draw(CommandRecorder& recorder);


};
