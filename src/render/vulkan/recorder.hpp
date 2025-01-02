#pragma once

#include "external.hpp"

class CommandRecorder {

	private:

		VkCommandBuffer vk_buffer;

	public:

		CommandRecorder(VkCommandBuffer vk_buffer);

		/**
		 * TODO documentation
		 */
		void done();

};
