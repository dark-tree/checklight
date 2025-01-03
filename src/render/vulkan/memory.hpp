#pragma once

#include "external.hpp"
#include "device.hpp"

class Memory {

	private:

		VkPhysicalDeviceMemoryProperties vk_properties;

		void printType(int index);
		void printHeap(int index);

	public:

		Memory() = default;
		Memory(PhysicalDevice& device);

		void print();

};
