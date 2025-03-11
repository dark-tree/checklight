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

		using bool32 = uint32_t;

		Type type;
		glm::vec3 position;
		glm::vec3 color;
		glm::vec3 direction;
		float intensity;
		bool32 shadow;

};

class LightManager {

	public:

		LightManager();

		void close();

		void addLight(Light light);

		void flush();

		const ReusableBuffer& getBuffer();

		friend class Renderer;

	private:

		std::vector<Light> lights;
		ReusableBuffer buffer;
		bool dirty;

		void flush(CommandRecorder& recorder);		

};
