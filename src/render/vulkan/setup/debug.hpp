#pragma once

#include "external.hpp"

class VulkanDebug {

	private:

		/// Constructs the struct used by insert() and begin()
		static VkDebugUtilsLabelEXT getDebugLabel(const char* debug_name, glm::vec3 color);

		/// Converts the VkObjectType enum into a human readable string
		static const char* getObjectName(VkObjectType type);

		static std::string getObjectFullname(VkObjectType type, const char* name);

	public:

		/**
		 * Mark the given object as alive
		 */
		static void beginLifetime(VkObjectType type, void* object, const char* debug_name);

		/**
		 * Mark the given object as dead
		 */
		static void endLifetime(void* object);

		/**
		 * Throws if the given object is not alive
		 */
		static void assertAlive(void* object);

		/**
		 * Throws if there is any object still alive
		 */
		static void assertAllDead();

		/**
		 * Adds a debug name visible in tools like RenderDoc to any Vulkan object
		 */
		static void setDebugName(VkDevice device, VkObjectType type, void* object, const char* debug_name);

		/**
		 * Adds a debug label (name + color) into the command buffer
		 */
		static void insertDebugLabel(VkCommandBuffer vk_buffer, const char* debug_name, glm::vec3 color);

		/**
		 * Begins a named debug block (name + color), all commands until matching end() will appear inside
		 */
		static void beginDebugBlock(VkCommandBuffer vk_buffer, const char* debug_name, glm::vec3 color);

		/**
		 * End the previously began named debug block
		 */
		static void endDebugBlock(VkCommandBuffer vk_buffer);

};
