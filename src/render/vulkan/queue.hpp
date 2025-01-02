#pragma once

#include "external.hpp"

class Queue {

	private:

		VkQueue vk_queue;

	public:

		Queue() = default;
		Queue(VkQueue queue);

};
