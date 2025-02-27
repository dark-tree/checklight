#pragma once

#include "external.hpp"
#include "clip.hpp"

class SoundSourceObject {
private:
	ALuint* sso_sources;
	ALsizei number_of_sources;
	
	void initSource(int number_of_sources);
	void play(int source_number);
	void stop(int source_number);
	void pause(int source_number);
public:
	SoundSourceObject();
	SoundSourceObject(int numberOfSources);
	~SoundSourceObject();

	ALuint getSource();
	ALuint getSource(int number);

	void setPosition(float x, float y, float z);

	void addBuffer(SoundClip buffer);
	void addBuffer(std::shared_ptr<SoundClip> buffer);
	void addBuffers(SoundClip buffer);

	void playSound();
	void stopSound();
	void pauseSound();

	bool isPlaying();
};
