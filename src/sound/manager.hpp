#pragma once

#include "external.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include "clip.hpp"
#include "source.hpp"

class SoundManager {
private:
	ALCdevice* p_ALCDevice;			//pointer to alc device
	ALCcontext* p_ALCContext;		//pointer to alc context

	std::unordered_map<std::string, std::shared_ptr<SoundClip>> ums_clips;
	std::unordered_map<std::string, std::shared_ptr<SoundSourceObject>> ums_sources;

	std::mutex sound_manager_mutex;
	std::thread sound_manager_thread;
	bool sound_loop_running = true;

	void init();

	SoundManager();
	~SoundManager();

	void soundManagerLoop();

public:
	static SoundManager& getInstance(){
		static SoundManager instance;
		return instance;
	}
	/*SoundManager();
	~SoundManager();*/
	ALCdevice* getDevice();
	ALCcontext* getContext();

	void createSource(const std::string& name);
	void createSource(const std::string& name,int source_size);

	void createClip(const std::string& name);
	void createClip(const std::string& name, int buffer_size);

	void addAudioToClip(const std::string& clip_name,const char* uri);

	void connectClipWithSource(const std::string& clip_name, const std::string& source_name);

	void playSound(const std::string& source_name);
};