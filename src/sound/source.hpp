#pragma once

#include "external.hpp"
#include "clip.hpp"

class SoundSourceObject {
private:
	ALuint* sso_sources;
	ALsizei number_of_sources;
public:
	SoundSourceObject();
	SoundSourceObject(int numberOfSources);
	~SoundSourceObject();

	ALuint getSource();
	ALuint getSource(int number);

	void addBuffer(SoundClip buffer);
	void addBuffers(SoundClip buffer);
};
