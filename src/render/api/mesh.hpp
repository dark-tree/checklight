#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/system.hpp"
#include "commander.hpp"

class RenderMesh {

	private:

		size_t vertex_count;
		Buffer vertex;

	public:

		~RenderMesh();

		template <typename V>
		void upload(RenderCommander& commander, const std::vector<V>& vertices) {
			vertex.close();
			vertex_count = vertices.size();

			size_t bytes = vertices.size() * sizeof(V);
			RenderSystem& system = *RenderSystem::system;

			VkBufferUsageFlags staged_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			Buffer staged = system.allocator.allocateBuffer(Memory::STAGED, bytes, staged_usage);

			VkBufferUsageFlags vertex_usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			vertex = system.allocator.allocateBuffer(Memory::DEVICE, vertices.size() * sizeof(V), vertex_usage);

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

		Buffer& getVertexBuffer();
		size_t getVertexCount();

};
