#pragma once

#include "external.hpp"
#include "shared/queue.hpp"
#include "render/vulkan/command/buffer.hpp"
#include "render/vulkan/descriptor/descriptor.hpp"
#include "render/vulkan/buffer/buffer.hpp"

class Renderer;

using bool32 = uint32_t;

struct SceneUniform {
	glm::mat4 view;
	glm::mat4 view_inv;
	glm::mat4 projection;
	glm::mat4 projection_inv;
	glm::mat4 prev_view;
	glm::mat4 prev_view_inv;
	glm::mat4 prev_projection;
	glm::mat4 prev_projection_inv;
	float time;
	float near;
	float far;
	glm::vec3 dir_light_direction;
	glm::vec3 dir_light_color;
	glm::vec3 ambient_color;
	int gi_samples;
	bool32 denoise;
	bool32 shadows;
};

class RenderFrame {

	public:

		CommandBuffer buffer;

		/// this needs to be before any BasicBuffer so that the internal mutex is ready
		/// before the BasicBuffer constructor runs as it uses `system.defer()`
		TaskQueue queue;

		/// this semaphore will be unlocked once the framebuffer returned from
		/// `RenderSystem::acquireFramebuffer()` is actually ready to be written to, see `CommandSubmitter::awaits()`
		Semaphore available_semaphore;

		/// this should be unlocked by the frame consumer using `CommandSubmitter::unlocks()`
		/// as `RenderSystem::presentFramebuffer()` waits for this semaphore to be unlocked
		Semaphore finished_semaphore;

		/// this should be unlocked by the frame consumer using `CommandSubmitter::unlocks()`
		/// and waited on using `wait()` before starting the rendering of a frame, it is used to keep CPU and GPU in sync
		Fence flight_fence;

		/// Small buffer used to store uniform variables
		/// after writing data to `uniforms` call uploadUniformBuffer()
		Buffer uniform_buffer;

		/// Contains all per-frame static data
		/// Like camera matrices and the like
		SceneUniform uniforms;

	public:

		SceneUniform* uniform_map;
		DescriptorSet set_immediate;
		DescriptorSet set_compose;
		DescriptorSet set_raytrace;
		DescriptorSet set_denoise;
		DescriptorSet set_denoise2;

	public:

		RenderFrame(Renderer& system, const CommandPool& pool, const LogicalDevice& device);

		/**
		 * This class is fully managed by the RenderSystem so it uses
		 * a destructor not a standard close() method
		 */
		~RenderFrame();

		/**
		 * Sends the updated uniform buffer content to the GPU memory,
		 * always call this method when you want to flush the uniforms
		 * after modifying the uniforms object
		 */
		void flushUniformBuffer(CommandRecorder& recorder);

		/**
		 * Wait before starting to render this frame
		 * until the last frame with this index has been completed
		 * (this is needed to keep the CPU from "running away" from the GPU).
		 */
		void wait();

		/**
		 * Execute all pending frame tasks, this usually is used
		 * for running cleanup hooks
		 */
		void execute();

};
