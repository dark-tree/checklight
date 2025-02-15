#include "sound.hpp"

SoundManager::SoundManager()
{
	p_ALCDevice = alcOpenDevice(nullptr);
	if (!p_ALCDevice) {
		throw std::runtime_error("OpenAL -> init: Failed to open device");  //throw exception
	}

	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);
	if (!p_ALCContext || !alcMakeContextCurrent(p_ALCContext)) {
		throw std::runtime_error("OpenAL -> init: Failed to load context on the device");  //throw exception
	}

	printf("OpenAL initialized successfully.\n");

}

SoundManager::~SoundManager()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(p_ALCContext);
	alcCloseDevice(p_ALCDevice);
}

ALCcontext* SoundManager::getContext()
{
	return this->p_ALCContext;
}

ALCdevice* SoundManager::getDevice()
{
	return this->p_ALCDevice();
}