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

void SoundManager::createSource(const std::string& name){

	// check if exist a source with a given name
	if (ums_sources.find(name) != ums_sources.end()){
		throw std::runtime_error("SoundManager -> createSource: SoundSourceObject with name: " + name + " already exist\f");
	}

	ums_sources[name] = std::make_shared<SoundSourceObject>();
}

void SoundManager::createSource(const std::string& name, int source_size){

	// check if exist a source with a given name
	if (ums_sources.find(name) != ums_sources.end()){
		throw std::runtime_error("SoundManager -> createSource: SoundSourceObject with name: " + name + " already exist\f");
	}

	ums_sources[name] = std::make_shared<SoundSourceObject>(source_size);
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

void SoundManager::connectClipWithSource(const std::string& clip_name, const std::string& sso_source_name) {
	// check if exist a clip with a given name
	if (ums_clips.find(clip_name) == ums_clips.end()) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundClip with name: " + clip_name + " not already exist\f");
	}

	// check if exist a source with a given name
	if (ums_sources.find(sso_source_name) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject with name: " + sso_source_name + " not already exist\f");
	}

	ums_sources[sso_source_name]->addBuffer(ums_clips[clip_name]);
}

void SoundManager::playSound(const std::string& sso_source_name){
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a source with a given name
	if (ums_sources.find(sso_source_name) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> playSound: SoundSourceObject with name: " + sso_source_name + " not already exist\f");
	}
	ums_sources[sso_source_name]->playSound();
}

void SoundManager::stopSound(const std::string& sso_source_name) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a source with a given name
	if (ums_sources.find(sso_source_name) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> stopSound: SoundSourceObject with name: " + sso_source_name + " not already exist\f");
	}
	ums_sources[sso_source_name]->stopSound();
}

void SoundManager::pauseSound(const std::string& sso_source_name) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a source with a given name
	if (ums_sources.find(sso_source_name) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> stopSound: SoundSourceObject with name: " + sso_source_name + " not already exist\f");
	}
	ums_sources[sso_source_name]->pauseSound();
}

ALCcontext* SoundManager::getContext(){
	return this->p_ALCContext;
}

//ALCdevice* SoundManager::getDevice()
//{
//	return this->p_ALCDevice();
//}