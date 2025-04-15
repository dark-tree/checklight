#pragma once
#include "external.hpp"

//@TODO poprawic wczytywanie plikow audio stereo, kilka bufferow dla jednego pliku, rozdzielic kanaly na mono 

/// Object to management of buffers
/// In this version provide management only on one buffer
class SoundClip {
private:

	/// Pointer to id of buffers
	ALuint buffer;
	/// Path of buffer in buffer
	std::string path;

	void loadOGGFile(const char* filename);
	void loadWAVFile(const char* filename);

public:

	SoundClip();
	~SoundClip();

	/// Method to create audio buffer for audio file
	/// Enable only .ogg files
	/// 
	/// @param path Path to the audio file
	void loadAudio(const char* path);

	/// Get the buffer
	ALuint getBuffer();

	/// Get Path from the buffer
	std::string getPath();
};