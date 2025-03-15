#pragma once

#include "external.hpp"
#include "clip.hpp"
#include "source.hpp"

// @TODO add debug command, improve throw and exception

class SoundManager {
private:

	// Pointer to the audio device 
	ALCdevice* p_ALCDevice;
	
	// Pointer to the openal context
	ALCcontext* p_ALCContext;


	//@TODO przerobic na vector <weak_ptr<SoundClip>>
	// @TODO przerobic na vector <weak_ptr<SoundSourceObject>>
	// Vector to storing sound clips 
	std::vector<std::weak_ptr<SoundClip>> v_clips;
	// Vector to storing sound source objects 
	std::vector<std::weak_ptr<SoundSourceObject>> v_sources;

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
	std::vector<std::weak_ptr<SoundSourceObject>> getVectorSoundSourceObject() { return v_sources; }
	std::vector<std::weak_ptr<SoundClip>> getVectorSoundClip() { return v_clips; }

	/// Add a SoundSourceObject to the vector v_sources
	/// @param sso Pointer to SoundSourceObject
	void addSource(std::shared_ptr<SoundSourceObject> sso);

	/// Add a SoundClip to the vector v_clips
	/// @param sc Pointer to SoundClip
	void addClip(std::shared_ptr <SoundClip> sc);

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

	
	void cleanupVectors();
};