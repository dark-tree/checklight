#pragma once

#include "external.hpp"
#include "render/api/reusable.hpp"

class LogicalDevice;
class CommandRecorder;
class TaskQueue;
class Allocator;
class Renderer;

struct Light {

	public:

		enum Type : uint32_t {
			DIRECTIONAL = 1,
			POINT = 2
		};

		using bool32_t = uint32_t;

		Type type;
		glm::vec3 vector;
		glm::vec3 color;
		float intensity;
		bool32_t shadow;

};

class LightManager {

	public:

		LightManager();

		void close();

		/**
		 * Create a new point light with the specified parameters and add it to the manager.
		 *
		 * @return Pointer to the newly created light
		 */
		std::shared_ptr<Light> createPointLight(glm::vec3 position, glm::vec3 color, float intensity, bool shadow);

		/**
		 * Create a new directional light with the specified parameters and add it to the manager.
		 *
		 * @return Pointer to the newly created light
		 */
		std::shared_ptr<Light> createDirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity, bool shadow);

		/**
		 * Add a light to the manager.
		 * The flush() operation is called automatically, so there is no need to call it manually.
		 *
		 * @param light Light to add
		 */
		void addLight(const std::shared_ptr<Light>& light);

		/**
		 * Remove a light from the manager
		 * The flush() operation is called automatically, so there is no need to call it manually.
		 *
		 * @param light Light to remove
		 */
		void removeLight(const std::shared_ptr<Light>& light);

		/**
		 * Flush the light buffer.
		 * Call this to apply changes to the lights.
		 */
		void flush();

		/**
		 * Get the buffer containing the lights
		 *
		 * @return Buffer containing the lights
		 */
		const ReusableBuffer& getBuffer();

		friend class Renderer;

	private:

		std::vector<std::shared_ptr<Light>> lights;
		ReusableBuffer buffer;
		bool dirty;

		void flush(CommandRecorder& recorder);		

};
