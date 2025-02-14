#pragma once

#include "render/vulkan/raytrace/struct.hpp"

class RenderModel {

	private:

		friend class AccelStructFactory;
		friend class Renderer;

		AccelStruct structure;
		VkDeviceAddress address;

	public:

		RenderModel(const LogicalDevice& device, const AccelStruct& structure);

		VkDeviceAddress getAddress() const;

		void setStructure(const LogicalDevice& device, const AccelStruct& structure);

};
