#include "manager.hpp"

SoundManager::SoundManager(){
	p_ALCDevice = alcOpenDevice(nullptr);
	if (!p_ALCDevice) {
		throw std::runtime_error("OpenAL -> init: Failed to open device\f");  //throw exception
	}

	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);
	if (!p_ALCContext || !alcMakeContextCurrent(p_ALCContext)) {
		throw std::runtime_error("OpenAL -> init: Failed to load context on the device\f");  //throw exception
	}

	printf("OpenAL initialized successfully.\n");

}

SoundManager::~SoundManager(){

	// delete ums
	ums_sources.clear();
	ums_clips.clear();

	// free device and context
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(p_ALCContext);
	alcCloseDevice(p_ALCDevice);
}

void SoundManager::addSource(std::shared_ptr <SoundSourceObject> sso){
	if (!sso) {
		throw std::runtime_error("SoundManager -> createSource: SoundSourceObject not exist\f");
	}

	// check if exist a source with a given name
	if (ums_sources.find(sso.get()) != ums_sources.end()){
		throw std::runtime_error("SoundManager -> createSource: SoundSourceObject already exist\f");
	}

	ums_sources[sso.get()] = sso;
}

void SoundManager::createClip(const std::string& name) {

	// check if exist a clip with a given name
	if (ums_clips.find(name) != ums_clips.end()) {
		throw std::runtime_error("SoundManager -> createClip: SoundClip with name: " + name + " already exist\f");
	}

	ums_clips[name] = std::make_shared<SoundClip>();
}

void SoundManager::createClip(const std::string& name, int clip_size) {

	// check if exist a clip with a given name
	if (ums_clips.find(name) != ums_clips.end()) {
		throw std::runtime_error("SoundManager -> createClip: SoundClip with name: " + name + " already exist\f");
	}

	ums_clips[name] = std::make_shared<SoundClip>(clip_size);
}

void SoundManager::addAudioToClip(const std::string& clip_name, const char* uri) {

	// check if exist a clip with a given name
	if (ums_clips.find(clip_name) == ums_clips.end()) {
		throw std::runtime_error("SoundManager -> addAudioToClip: SoundClip with name: " + clip_name + " not already exist\f");
	}

	ums_clips[clip_name]->addAudio(uri);
}

void SoundManager::connectClipWithSource(const std::string& clip_name, std::shared_ptr <SoundSourceObject> sso) {
	if (!sso) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject not exist\f");
	}
	// check if exist a clip with a given name
	if (ums_clips.find(clip_name) == ums_clips.end()) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundClip with name: " + clip_name + " not already exist\f");
	}

	// check if exist a source with a given name
	if (ums_sources.find(sso.get()) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject not already exist in ums_sources. Please add to map (sm.addSource(sso))\f");
	}

	ums_sources[sso.get()]->addBuffer(ums_clips[clip_name]);
}

void SoundManager::playSound(std::shared_ptr <SoundSourceObject> sso){
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	if (!sso) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject not exist\f");
	}
	// check if exist a source with a given name
	if (ums_sources.find(sso.get()) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> playSound: SoundSourceObject not already exist in ums_sources. Please add to map (sm.addSource(sso))\f");
	}
	ums_sources[sso.get()]->playSound();
}

void SoundManager::stopSound(std::shared_ptr <SoundSourceObject> sso) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);
	if (!sso) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject not exist\f");
	}
	// check if exist a source with a given name
	if (ums_sources.find(sso.get()) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> stopSound: SoundSourceObject not already exist in ums_sources. Please add to map (sm.addSource(sso))\f");
	}
	ums_sources[sso.get()]->stopSound();
}

void SoundManager::pauseSound(std::shared_ptr <SoundSourceObject> sso) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);
	if (!sso) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject not exist\f");
	}
	// check if exist a source with a given name
	if (ums_sources.find(sso.get()) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> stopSound: SoundSourceObject not already exist in ums_sources. Please add to map (sm.addSource(sso))\f");
	}
	ums_sources[sso.get()]->pauseSound();
}

ALCcontext* SoundManager::getContext(){
	return this->p_ALCContext;
}

//ALCdevice* SoundManager::getDevice()
//{
//	return this->p_ALCDevice();
//}