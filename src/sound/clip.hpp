#pragma once

#include "external.hpp"

class SoundClip {
private:
	ALuint* sc_buffers;
	ALsizei number_of_buffers;
public:
	SoundClip();
	SoundClip(int number_of_buffers);
	~SoundClip();

	void addClip(const char* filename);
	ALuint getBuffer(int number);
};