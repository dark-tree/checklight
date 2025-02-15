#pragma once

#include "external.hpp"
#include <AL/al.h>
#include <AL/alc.h>

class SoundManager {
private:
	ALCdevice* p_ALCDevice;			//pointer to alc device
	ALCcontext* p_ALCContext;		//pointer to alc context

	void init();

	SoundManager();
	~SoundManager();
public:
	static SoundManager& getInstance()
	{
		static SoundManager instance;
		return instance;
	}

	ALCdevice* getDevice();
	ALCcontext* getContext();
};