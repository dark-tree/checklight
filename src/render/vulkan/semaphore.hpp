#pragma once

#include "external.hpp"

class Semaphore {

	private:

		VkDevice vk_device;
		VkSemaphore vk_semaphore;

	public:

		Semaphore(VkDevice vk_device);

		void close();
		VkSemaphore getHandle() const;

};
