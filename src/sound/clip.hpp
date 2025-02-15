#pragma once

#include "external.hpp"

class SoundClip {
private:
	ALuint* SCbuffers;
	ALsizei numberOfBuffers;
public:
	SoundClip();
	SoundClip(int numberOfBuffers);
	~SoundClip();
};