#pragma once

#include "render/vulkan/command/recorder.hpp"
#include "render/vulkan/command/buffer.hpp"
#include "shared/queue.hpp"

class RenderCommander {

	public:

		enum Type {
			TRANSIENT,
			ASYNCHRONOUS,
		};

	private:

		Type type;
		CommandBuffer buffer;
		CommandRecorder recorder;
		TaskQueue queue;

	public:

		RenderCommander(Type type, CommandBuffer buffer, CommandRecorder recorder, TaskQueue queue);

		CommandRecorder& getRecorder();
		TaskQueue& getTaskQueue();

		/**
		 * After you are done using the commander ALWAYS call
		 * this method, any other action is INVALID!
		 */
		void complete();

};
