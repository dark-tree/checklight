#pragma once

#include "external.hpp"
#include "clip.hpp"
#include "source.hpp"

//@TODO Rebuild soundClip

class SoundManager {
private:

	// Pointer to the audio device 
	ALCdevice* p_ALCDevice;
	
	// Pointer to the openal context
	ALCcontext* p_ALCContext;

	// Unordered maps to storing sound clips with their names as a key 
	std::unordered_map<SoundClip*, std::shared_ptr<SoundClip>> ums_clips;
	std::unordered_map<SoundSourceObject*, std::shared_ptr<SoundSourceObject>> ums_sources;

	// Tools for managing the sound thread
	std::mutex sound_manager_mutex;

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

	/// Add a SoundSourceObject to the unordermap ums_sources
	/// @param sso Pointer to SoundSourceObject
	void addSource(std::shared_ptr<SoundSourceObject> sso);

	/// Add a SoundSourceObject to the unordermap ums_sources
	/// @param sso Pointer to SoundSourceObject
	void addClip(std::shared_ptr<SoundClip> sc);

	/*
	/// Create a new SoundClip
	/// Add this object to ums_clips
	void createClip();
	/// Create a new SoundClip with a given name (default size: 1)
	/// Add this object to ums_clips
	/// @param name Name of new SoundClip
	void createClip(const std::string& name);
	/// Create a new SoundClip with a given name and size (default: 1)
	/// Add this object to ums_clips
	/// @param name Name of new SoundClip
	/// @param buffer_size Size of buffers table in SoundClip
	void createClip(const std::string& name, int buffer_size);
	*/

	/*
	/// Load an OGG file from the given URI and add it to buffer[0] in SoundClip with clip_name
	/// @param clip_name Name of SoundClip (key in ums_clips)
	/// @param uri Path to the OGG file
	void addAudioToClip(const std::string& clip_name,const char* uri);
	*/

	/// Load an OGG file from the given URI and add it to buffer[0] in SoundClip
	/// @param sc std::shared_ptr SoundClip object
	/// @param uri Path to the OGG file
	void addAudioToClip(std::shared_ptr<SoundClip> sc, const char* uri);

	/// Get a SoundClip with clip_name and add the first buffer from SoundClip to the SoundSourceObject
	/// @param sc std::shared_ptr SoundClip object
	/// @param sso std::shared_ptr SoundSourceObject object
	void connectClipWithSource(std::shared_ptr<SoundClip> sc, std::shared_ptr <SoundSourceObject> sso);

	/// Sound play control funtion
	/// @param sso SoundSourceObject object
	void playSound(std::shared_ptr <SoundSourceObject> sso);
	/// Sound stop control funtion
	/// @param sso SoundSourceObject object
	void stopSound(std::shared_ptr <SoundSourceObject> sso);
	/// Sound pause control funtion
	/// @param sso SoundSourceObject object
	void pauseSound(std::shared_ptr <SoundSourceObject> sso);
};