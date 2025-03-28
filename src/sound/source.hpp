#pragma once

#include "external.hpp"
#include "clip.hpp"
#include "group.hpp"

class SoundSourceObject {
private:
	ALuint* sso_sources;
	ALsizei number_of_sources;
	
	std::shared_ptr<SoundGroup> sso_sg;
	
	std::weak_ptr<SoundClip> sc_buffer;

	void initSource(int number_of_sources);
	void play(int source_number);
	void stop(int source_number);
	void pause(int source_number);

public:
	// SoundSourceObject constructor to create object with only one sound source
	SoundSourceObject();

	// SoundSourceObject constructor to create a many sound sources in one object
	//SoundSourceObject(int numberOfSources);

	~SoundSourceObject();

	ALuint getSource();
	ALuint getSource(int number);

	void setPosition(float x, float y, float z);

	void addBuffer(SoundClip buffer);
	void addBuffer(std::shared_ptr<SoundClip> buffer);
	//void addBuffers(SoundClip buffer);

	void addGroupParameters(std::shared_ptr <SoundGroup> sg);
	void updateParameters();
	void updateMovement();

	void playSound();
	void stopSound();
	void pauseSound();

	bool isPlaying();
};
