#pragma once
#include "external.hpp"

//@TODO poprawic wczytywanie plikow audio stereo, kilka bufferow dla jednego pliku, rozdzielic kanaly na mono 

// Object to management of buffers
//In this version provide management only on one buffer
class SoundClip {
private:

	// Pointer to id of buffers
	ALuint* sc_buffers;
	// Number of buffers (enable only default value = 1)
	ALsizei number_of_buffers;
	// Uri of first buffer in sc_buffers
	std::string uri;

public:

	SoundClip();
	//SoundClip(int number_of_buffers);
	~SoundClip();

	// Create buffer for audio file
	// Only for .ogg file
	// @param uri URI to the audio file
	void addAudio(const char* uri);

	//ALuint getBuffer(int number);

	// Get first buffer
	ALuint getBuffer();

	// Get URI from first buffer
	std::string getURI();
};