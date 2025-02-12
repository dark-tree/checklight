#pragma once

#include "render/vulkan/raytrace/struct.hpp"

class RenderModel {

	private:

		friend class AccelStructFactory;
		friend class Renderer;

		AccelStruct structure;
		VkDeviceAddress address;

	public:

		RenderModel(const LogicalDevice& device, AccelStruct structure);

		VkDeviceAddress getAddress() const;

};
