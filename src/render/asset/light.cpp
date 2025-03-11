
#include "light.hpp"

/**
 * LightManager
 */

LightManager::LightManager() 
: buffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT), dirty(false) {
	buffer.setDebugName("Light Buffer");

	Light light {};
	light.type = static_cast<Light::Type>(0);

	addLight(light);

}

void LightManager::close() {
	buffer.close();
}

void LightManager::flush() {
	dirty = true;
}

void LightManager::addLight(Light light) {
	// new light is always added at the front
	lights.insert(lights.begin(), light);
	flush();
}

const ReusableBuffer& LightManager::getBuffer() {
	return buffer;
}

void LightManager::flush(CommandRecorder& recorder) {
	if (dirty) {
		buffer.resize(lights.size(), sizeof(Light));
		buffer.writeToStaging(lights.data(), lights.size(), sizeof(Light), 0);
		buffer.flushStaging(recorder);
		dirty = false;
	}
}
