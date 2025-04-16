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

protected:

	/// Load ogg file to buffer
	void loadOGGFile(const char* filename);
	/// Load wav file to buffer
	void loadWAVFile(const char* filename);

public:

	SoundClip();
	~SoundClip();

	/// Method to create audio buffer for audio file
	/// If the audio file has more than 1 channel, converts to mono
	/// by extracting only one channel (first channel)
	/// If you want to use other method to convert, overwrite loadOGGFile or loadWAVFile
	/// Enable only OGG and WAV files
	/// 
	/// @param path Path to the audio file
	void loadAudio(const char* path);

	/// Get the buffer
	ALuint getBuffer();

	/// Get Path from the buffer
	std::string getPath();
};