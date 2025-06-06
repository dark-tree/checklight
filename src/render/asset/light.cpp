
#include "light.hpp"

/**
 * LightManager
 */

LightManager::LightManager() 
: buffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT), dirty(false) {
	buffer.setDebugName("Light Buffer");

}

void LightManager::close() {
	buffer.close();
}

void LightManager::flush() {
	dirty = true;
}

std::shared_ptr <Light> LightManager::createPointLight(glm::vec3 position, glm::vec3 color, float intensity, bool shadow) {
	auto ptr = std::make_shared<Light>(Light::POINT, position, color, intensity, shadow);
	addLight(ptr);
	return ptr;
}

std::shared_ptr <Light> LightManager::createDirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity, bool shadow) {
	auto ptr = std::make_shared<Light>(Light::DIRECTIONAL, direction, color, intensity, shadow);
	addLight(ptr);
	return ptr;
}

void LightManager::addLight(const std::shared_ptr<Light>& light) {
	// new light is always added at the front
	lights.push_back(light);
	flush();
}

void LightManager::removeLight(const std::shared_ptr<Light>& light) {
	auto it = std::find(lights.begin(), lights.end(), light);

	if (it != lights.end()) {
		lights.erase(it);
		flush();
	}
}

const ReusableBuffer& LightManager::getBuffer() {
	return buffer;
}

void LightManager::flush(CommandRecorder& recorder) {
	if (dirty) {
		// copy all lights to the temporary array
		std::vector<Light> lights_data(lights.size() + 1);
		for (size_t i = 0; i < lights.size(); i++) {
			lights_data[i] = *lights[i];
		}

		// add a dummy light at the end
		Light light{};
		light.type = static_cast<Light::Type>(0);
		lights_data[lights.size()] = light;

		// write to buffer
		buffer.resize(lights_data.size(), sizeof(Light));
		buffer.writeToStaging(lights_data.data(), lights_data.size(), sizeof(Light), 0);
		buffer.flushStaging(recorder);
		dirty = false;
	}
}
