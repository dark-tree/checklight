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

	// start sound_manager_thread
	sound_manager_thread = std::thread(&SoundManager::soundManagerLoop, this);
	printf("OpenAL initialized successfully.\n");

}

SoundManager::~SoundManager(){

	// stop an infinitie loop
	sound_loop_running = false;

	// close sound_manager_thread
	if (sound_manager_thread.joinable()){
		sound_manager_thread.join();
	}

	// delete ums
	ums_sources.clear();
	ums_clips.clear();

	// free device and context
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(p_ALCContext);
	alcCloseDevice(p_ALCDevice);
}

void SoundManager::soundManagerLoop(){
	while (sound_loop_running){
		//std::lock_guard<std::mutex> lock(sound_manager_mutex);
		
		/*ALint state;
		do {
		alGetSourcei(sso_sources[0], AL_SOURCE_STATE, &state);
		} while (state == AL_PLAYING);*/

		/*for (auto& sso : vs_sources){
			if (!sso->isPlaying()) {

			}
		}*/

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void SoundManager::createSource(const std::string& name){
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a source with a given name
	if (ums_sources.find(name) != ums_sources.end()){
		throw std::runtime_error("SoundManager -> createSource: SoundSourceObject with name: " + name + " already exist\f");
	}

	ums_sources[name] = std::make_shared<SoundSourceObject>();
}

void SoundManager::createSource(const std::string& name, int source_size){
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a source with a given name
	if (ums_sources.find(name) != ums_sources.end()){
		throw std::runtime_error("SoundManager -> createSource: SoundSourceObject with name: " + name + " already exist\f");
	}

	ums_sources[name] = std::make_shared<SoundSourceObject>(source_size);
}

void SoundManager::createClip(const std::string& name) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a clip with a given name
	if (ums_clips.find(name) != ums_clips.end()) {
		throw std::runtime_error("SoundManager -> createClip: SoundClip with name: " + name + " already exist\f");
	}

	ums_clips[name] = std::make_shared<SoundClip>();
}

void SoundManager::createClip(const std::string& name, int clip_size) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a clip with a given name
	if (ums_clips.find(name) != ums_clips.end()) {
		throw std::runtime_error("SoundManager -> createClip: SoundClip with name: " + name + " already exist\f");
	}

	ums_clips[name] = std::make_shared<SoundClip>(clip_size);
}

void SoundManager::addAudioToClip(const std::string& clip_name, const char* uri) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a clip with a given name
	if (ums_clips.find(clip_name) == ums_clips.end()) {
		throw std::runtime_error("SoundManager -> addAudioToClip: SoundClip with name: " + clip_name + " not already exist\f");
	}

	ums_clips[clip_name]->addAudio(uri);
}

void SoundManager::connectClipWithSource(const std::string& clip_name, const std::string& source_name) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a clip with a given name
	if (ums_clips.find(clip_name) == ums_clips.end()) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundClip with name: " + clip_name + " not already exist\f");
	}

	// check if exist a source with a given name
	if (ums_sources.find(source_name) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject with name: " + source_name + " not already exist\f");
	}

	ums_sources[source_name]->addBuffer(ums_clips[clip_name]);
}

void SoundManager::playSound(const std::string& source_name){
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a source with a given name
	if (ums_sources.find(source_name) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> playSound: SoundSourceObject with name: " + source_name + " not already exist\f");
	}
	ums_sources[source_name]->playSound();
}

void SoundManager::stopSound(const std::string& source_name) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a source with a given name
	if (ums_sources.find(source_name) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> stopSound: SoundSourceObject with name: " + source_name + " not already exist\f");
	}
	ums_sources[source_name]->stopSound();
}

void SoundManager::pauseSound(const std::string& source_name) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);

	// check if exist a source with a given name
	if (ums_sources.find(source_name) == ums_sources.end()) {
		throw std::runtime_error("SoundManager -> stopSound: SoundSourceObject with name: " + source_name + " not already exist\f");
	}
	ums_sources[source_name]->pauseSound();
}

ALCcontext* SoundManager::getContext(){
	return this->p_ALCContext;
}

//ALCdevice* SoundManager::getDevice()
//{
//	return this->p_ALCDevice();
//}