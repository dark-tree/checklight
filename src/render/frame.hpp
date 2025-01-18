#pragma once

#include "external.hpp"
#include "util/queue.hpp"
#include "render/vulkan/command.hpp"
#include "render/vulkan/descriptor/descriptor.hpp"

class Renderer;

class RenderFrame {

	public:

		CommandBuffer buffer;

		/// this needs to be before any BasicBuffer so that the internal mutex is ready
		/// before the BasicBuffer constructor runs as it uses `system.defer()`
		TaskQueue queue;

		/// this semaphore will be unlocked once the frambuffer returned from
		/// `RenderSystem::acquireFramebuffer()` is actually ready to be written to, see `CommandSubmitter::awaits()`
		Semaphore available_semaphore;

		/// this should be unlocked by the frame consumer using `CommandSubmitter::unlocks()`
		/// as `RenderSystem::presentFramebuffer()` waits for this semaphore to be unlocked
		Semaphore finished_semaphore;

		/// this should be unlocked by the frame consumer using `CommandSubmitter::unlocks()`
		/// and waited on using `wait()` before starting the rendering of a frame, it is used to keep CPU and GPU in sync
		Fence flight_fence;

	public:

		DescriptorSet set_0;

	public:

		RenderFrame(Renderer& system, const CommandPool& pool, const LogicalDevice& device);

		/**
		 * This class is fully managed by the RenderSystem so it uses
		 * a destructor not a standard close() method
		 */
		~RenderFrame();

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
