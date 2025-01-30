
#include "debug.hpp"
#include "proxy.hpp"

static std::unordered_map<void*, std::string>* objects = new std::unordered_map<void*, std::string> {};

VkDebugUtilsLabelEXT VulkanDebug::getDebugLabel(const char* debug_name, glm::vec3 color) {
	VkDebugUtilsLabelEXT label {};
	label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	label.pNext = nullptr;
	label.pLabelName = debug_name;
	label.color[0] = color.r;
	label.color[1] = color.g;
	label.color[2] = color.b;
	label.color[3] = 1.0f;

	return label;
}

const char* VulkanDebug::getObjectName(VkObjectType type) {
	#if ENGINE_DEBUG
	if (type == VK_OBJECT_TYPE_INSTANCE) return "Instance";
	if (type == VK_OBJECT_TYPE_PHYSICAL_DEVICE) return "Physical Device";
	if (type == VK_OBJECT_TYPE_DEVICE) return "Device";
	if (type == VK_OBJECT_TYPE_QUEUE) return "Queue";
	if (type == VK_OBJECT_TYPE_SEMAPHORE) return "Semaphore";
	if (type == VK_OBJECT_TYPE_COMMAND_BUFFER) return "Command Buffer";
	if (type == VK_OBJECT_TYPE_FENCE) return "Fence";
	if (type == VK_OBJECT_TYPE_DEVICE_MEMORY) return "Device Memory";
	if (type == VK_OBJECT_TYPE_BUFFER) return "Buffer";
	if (type == VK_OBJECT_TYPE_IMAGE) return "Image";
	if (type == VK_OBJECT_TYPE_EVENT) return "Event";
	if (type == VK_OBJECT_TYPE_QUERY_POOL) return "Query Pool";
	if (type == VK_OBJECT_TYPE_BUFFER_VIEW) return "Buffer View";
	if (type == VK_OBJECT_TYPE_IMAGE_VIEW) return "Image View";
	if (type == VK_OBJECT_TYPE_SHADER_MODULE) return "Shader Module";
	if (type == VK_OBJECT_TYPE_PIPELINE_CACHE) return "Pipeline Cache";
	if (type == VK_OBJECT_TYPE_PIPELINE_LAYOUT) return "Pipeline Layout";
	if (type == VK_OBJECT_TYPE_RENDER_PASS) return "Render Pass";
	if (type == VK_OBJECT_TYPE_PIPELINE) return "Pipeline";
	if (type == VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT) return "Descriptor Set Layout";
	if (type == VK_OBJECT_TYPE_SAMPLER) return "Sampler";
	if (type == VK_OBJECT_TYPE_DESCRIPTOR_POOL) return "Descriptor Pool";
	if (type == VK_OBJECT_TYPE_DESCRIPTOR_SET) return "Descriptor Set";
	if (type == VK_OBJECT_TYPE_FRAMEBUFFER) return "Framebuffer";
	if (type == VK_OBJECT_TYPE_COMMAND_POOL) return "Command Pool";
	#endif

	return "Unknown";
}

void VulkanDebug::beginLifetime(void* object, const char* debug_name) {
	#if ENGINE_DEBUG
	if (objects->contains(object)) {
		throw std::runtime_error {"Can't begin lifetime, object already alive!"};
	}

	(*objects)[object] = debug_name;
	#endif
}

void VulkanDebug::endLifetime(void* object) {
	#if ENGINE_DEBUG
	if (!objects->contains(object)) {
		printf("DEBUG: Failed to kill %p\n", object);
		throw std::runtime_error {"Can't end lifetime, object already dead!"};
	}

	objects->erase(object);
	#endif
}

void VulkanDebug::assertAlive(void* object) {
	#if ENGINE_DEBUG
	if (!objects->contains(object)) {
		throw std::runtime_error {"Object lifetime assertion failed, object is dead!"};
	}
	#endif
}

void VulkanDebug::assertAllDead() {
	#if ENGINE_DEBUG
	if (!objects->empty()) {
		printf("ERROR: An 'All Dead' assertion reached but:\n");

		for (auto& pair : *objects) {
			printf(" * Object %s is still alive!\n", pair.second.c_str());
		}

		throw std::runtime_error {"Some object still live!"};
	}
	#endif
}

void VulkanDebug::setDebugName(VkDevice vk_device, VkObjectType type, void* object, const char* debug_name) {
	#if ENGINE_DEBUG
	if (debug_name == nullptr) {
		return;
	}

	std::string fullname = getObjectName(type);
	fullname.push_back(' ');
	fullname.push_back('\'');
	fullname += debug_name;
	fullname.push_back('\'');

	VkDebugUtilsObjectNameInfoEXT name_info {};
	name_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	name_info.objectType = type;
	name_info.objectHandle = (std::intptr_t) object;
	name_info.pObjectName = fullname.c_str();

	Proxy::vkSetDebugUtilsObjectNameEXT(vk_device, &name_info);
	#endif
}

void VulkanDebug::insertDebugLabel(VkCommandBuffer vk_buffer, const char* debug_name, glm::vec3 color) {
	#if ENGINE_DEBUG
	VkDebugUtilsLabelEXT label = getDebugLabel(debug_name, color);
	Proxy::vkCmdInsertDebugUtilsLabelEXT(vk_buffer, &label);
	#endif
}

void VulkanDebug::beginDebugBlock(VkCommandBuffer vk_buffer, const char* debug_name, glm::vec3 color) {
	#if ENGINE_DEBUG
	VkDebugUtilsLabelEXT label = getDebugLabel(debug_name, color);
	Proxy::vkCmdBeginDebugUtilsLabelEXT(vk_buffer, &label);
	#endif
}

void VulkanDebug::endDebugBlock(VkCommandBuffer vk_buffer) {
	#if ENGINE_DEBUG
	Proxy::vkCmdEndDebugUtilsLabelEXT(vk_buffer);
	#endif
}