#pragma once

#include "external.hpp"
#include "clip.hpp"
#include "source.hpp"
#include "functions.hpp"

class SoundManager {
private:

	/// Pointer to the audio device 
	ALCdevice* p_ALCDevice=nullptr;
	/// Pointer to the audio context
	ALCcontext* p_ALCContext=nullptr;


	/// Vector to storing sound clips 
	/// Clips are stored as shared_ptr to allow multiple ownership
	/// They must be deleted when we dont want to use them anymore not when they dont have any owner
	std::vector<std::shared_ptr<SoundClip>> v_clips;
	/// Vector to storing sound source objects 
	std::vector<std::weak_ptr<SoundSourceObject>> v_sources;
	/// Vector to storing sound groups
	std::vector<std::weak_ptr<SoundGroup>> v_groups;

	/// Constructor
	SoundManager();
	/// Destructor
	~SoundManager();

	/// Function to check if the object exist in the vector
	void checkIfExistInVector(const std::string& error_text, std::vector<std::weak_ptr<SoundSourceObject>>& vector, const std::shared_ptr<SoundSourceObject>& sso);

public:
	
	// Make SoundManager as singleton
	static SoundManager& getInstance(){
		static SoundManager instance;
		return instance;
	}

	ALCdevice* getDevice() { return p_ALCDevice; }
	ALCcontext* getContext() { return p_ALCContext; }

	/// Get vector of SoundSourceObject
	const std::vector<std::weak_ptr<SoundSourceObject>>& getVectorSoundSourceObject() { return v_sources; }
	/// Get vector of SoundClip
	const std::vector<std::shared_ptr<SoundClip>>& getVectorSoundClip() { return v_clips; }
	/// Get vector of SoundGroup
	const std::vector<std::weak_ptr<SoundGroup>>& getVectorSoundGroup() { return v_groups; }

	/// Add a SoundSourceObject to the vector v_sources
	/// 
	/// @param sso Pointer to SoundSourceObject
	void addSource(const std::shared_ptr<SoundSourceObject>& sso);

	/// Add a SoundClip to the vector v_clips
	/// 
	/// @param sc Pointer to SoundClip
	void addClip(const std::shared_ptr <SoundClip>& sc);

	/// Remove a SoundClib object
	/// 
	/// @param sc Shared pointer to SoundClip
	void removeClip(const std::shared_ptr <SoundClip>& sc);

	/// Add a SoundGroup to the vector v_groups
	/// 
	/// @param sg Pointer to SoundGroup
	void addGroup(const std::shared_ptr <SoundGroup>& sg);

	/// Load an OGG file from the given URI and add it to buffer[0] in SoundClip
	/// 
	/// @param sc std::shared_ptr SoundClip object
	/// @param path Path to the OGG file
	void loadAudioToClip(const std::shared_ptr<SoundClip>& sc, const char* path);

	/// Get a SoundClip with clip_name and add the first buffer from SoundClip to the SoundSourceObject
	/// 
	/// @param sc std::shared_ptr SoundClip object
	/// @param sso std::shared_ptr SoundSourceObject object
	void connectClipWithSource(const std::shared_ptr<SoundClip>& sc,const std::shared_ptr <SoundSourceObject>& sso);

	/// Create a new SoundClip, add it to the vector v_clips, load the audio file from the given URL and add it to the SoundSourceObject
	/// 
	/// @param path Path to the audio file
	/// @return std::shared_ptr to the SoundClip object.
	/// Do not get std::weak_ptr from this function, because it will be deleted after the function call
	std::shared_ptr<SoundClip> createSoundClipAndLoadAudio(const char* path);

	/// Create a new SoundClip, add it to the vector v_clips, load the audio file from the given URL and add it to the SoundSourceObject
	/// 
	/// @param path Path to the audio file
	/// @param sso std::shared_ptr SoundSourceObject object
	/// 
	/// @return std::shared_ptr to the SoundClip object.
	std::shared_ptr<SoundClip> createSoundClipAndAddToSourceObject(const char* path,const std::shared_ptr<SoundSourceObject>& sso);

	/// Sound play control funtion
	/// 
	/// @param sso SoundSourceObject object
	void playSound(const std::weak_ptr <SoundSourceObject>& sso);

	/// Sound stop control funtion
	/// 
	/// @param sso SoundSourceObject object
	void stopSound(const std::shared_ptr <SoundSourceObject>& sso);

	/// Sound pause control funtion
	/// 
	/// @param sso SoundSourceObject object
	void pauseSound(const std::shared_ptr <SoundSourceObject>& sso);
	
	/// Remove every expired pointer from vectors
	void cleanupVectors();
};
