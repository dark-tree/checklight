#pragma once

#include "external.hpp"
#include "clip.hpp"
#include "source.hpp"

class SoundManager {
private:

	// Pointer to the audio device 
	ALCdevice* p_ALCDevice;
	
	// Pointer to the openal context
	ALCcontext* p_ALCContext;

	// Unordered maps to storing sound clips and sound sources with their names as a key 
	std::unordered_map<std::string, std::shared_ptr<SoundClip>> ums_clips;
	std::unordered_map<std::string, std::shared_ptr<SoundSourceObject>> ums_sources;

	// Tools for managing the sound thread
	std::mutex sound_manager_mutex;
	std::thread sound_manager_thread;
	bool sound_loop_running = true;

	/// Loop function running in a separeted thread
	/// This function calls sleep(100ms) to keep the thread alive
	void soundManagerLoop();

	// Constructor
	SoundManager();
	// Destructor
	~SoundManager();

public:
	
	// Make SoundManager as singleton
	static SoundManager& getInstance(){
		static SoundManager instance;
		return instance;
	}

	//ALCdevice* getDevice();
	ALCcontext* getContext();

	/// Create a new SoundSourceObject with a given name and size (default: 1)
	/// Add this object to ums_sources
	/// @param name Name of new SoundSourceObject
	/// @param source_size Size of sources table in SoundSourceObject
	void createSource(const std::string& name);
	void createSource(const std::string& name,int source_size);

	/// Create a new SoundClip with a given name and size (default: 1)
	/// Add this object to ums_clips
	/// @param name Name of new SoundClip
	/// @param buffer_size Size of buffers table in SoundClip
	void createClip(const std::string& name);
	void createClip(const std::string& name, int buffer_size);

	/// Load an OGG file from the given URI and add it to buffer[0] in SoundClip with clip_name
	/// @param clip_name Name of SoundClip (key in ums_clips)
	/// @param uri Path to the OGG file
	void addAudioToClip(const std::string& clip_name,const char* uri);

	/// Get a SoundClip with clip_name and add the first buffer from SoundClip to the SoundSourceObject
	/// @param clip_name
	/// @param source_name
	void connectClipWithSource(const std::string& clip_name, const std::string& source_name);

	/// Sound control funtions
	/// @param source_name Name of SoundSourceObject
	void playSound(const std::string& source_name);
	void stopSound(const std::string& source_name);
	void pauseSound(const std::string& source_name);
};