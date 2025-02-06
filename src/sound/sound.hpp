#pragma once

#include "external.hpp"
#include <AL/al.h>
#include <AL/alc.h>

class SoundObject {
private:
	ALCdevice* p_ALCDevice;			//pointer to alc device
	ALCcontext* p_ALCContext;		//pointer to alc context

	void init();
public:
	SoundObject();
	~SoundObject();
};