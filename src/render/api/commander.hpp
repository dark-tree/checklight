#pragma once

#include "render/vulkan/command/recorder.hpp"
#include "render/vulkan/command/buffer.hpp"
#include "shared/queue.hpp"

/**
 * Represents a stream of operations issues to the GPU, if a method (or any
 * other operation) takes a RenderCommander as a parameter it can't be considered
 * completed until the RenderCommander itself is completed.
 */
class RenderCommander {

	private:

		CommandBuffer buffer;
		CommandRecorder recorder;
		TaskQueue queue;

	public:

		RenderCommander(CommandBuffer buffer, CommandRecorder recorder, TaskQueue queue);

		/// Get the underlying vulkan command buffer recorder
		CommandRecorder& getRecorder();

		/// Get the underlying deferred execution queue.
		TaskQueue& getTaskQueue();

		/**
		 * After you are done using the commander ALWAYS call
		 * this method, any other action is INVALID!
		 */
		void complete();

};
