#pragma once
#include "external.hpp"

//@TODO poprawic wczytywanie plikow audio stereo, kilka bufferow dla jednego pliku, rozdzielic kanaly na mono 

/// Object to management of buffers
/// In this version provide management only on one buffer
class SoundClip {
private:

	/// Pointer to id of buffers
	ALuint* sc_buffers;
	/// Number of buffers (enable only default value = 1)
	ALsizei number_of_buffers;
	/// Uri of first buffer in sc_buffers
	std::string url;

	void loadOGGFile(const char* filename);
	void loadWAVFile(const char* filename);

public:

	SoundClip();
	~SoundClip();

	/// Method to create audio buffer for audio file
	/// Enable only .ogg files
	/// 
	/// @param uri URL to the audio file
	void addAudio(const char* url);

	//ALuint getBuffer(int number);

	/// Gets the buffer id of the first buffer in sc_buffers
	ALuint getBuffer();

	/// Get URL from the first buffer in sc_buffers
	std::string getURL();
};