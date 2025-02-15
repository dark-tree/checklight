#pragma once

#include "external.hpp"
#include "sound.hpp"

class SoundSourceObject {
private:
	ALuint* SSOsources;
	ALsizei numberOfSources;
public:
	SoundSourceObject();
	SoundSourceObject(int numberOfSources);
	~SoundSourceObject();

	ALuint getSource();
	ALuint getSource(int number);
};
