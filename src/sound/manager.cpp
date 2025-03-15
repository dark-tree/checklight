#include "manager.hpp"

SoundManager::SoundManager(){
	p_ALCDevice = alcOpenDevice(nullptr);
	if (!p_ALCDevice) {
		throw std::runtime_error("OpenAL -> init: Failed to open device\f");  //throw exception
	}

	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);
	if (!p_ALCContext || !alcMakeContextCurrent(p_ALCContext)) {
		alcCloseDevice(p_ALCDevice);
		throw std::runtime_error("OpenAL -> init: Failed to load context on the device\f");  //throw exception
	}

	printf("OpenAL initialized successfully.\n");

}

SoundManager::~SoundManager(){

	// delete vectors
	v_sources.clear();
	v_clips.clear();

	// free device and context
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(p_ALCContext);
	alcCloseDevice(p_ALCDevice);

}

template <typename T>
auto find_in_vector(std::vector< std::weak_ptr<T>>& vec, std::shared_ptr<T> spt) {
	return std::find_if(vec.begin(), vec.end(),
		[spt](const std::weak_ptr<T>& wp) {
			return !wp.expired() && wp.lock() == spt;
		});
}

void SoundManager::addSource(std::shared_ptr <SoundSourceObject> sso){
	if (!sso) {
		std::cerr<<("SoundManager -> addSource: SoundSourceObject not exist\f");
		return;
	}

	// check if exist a source with a given name
	if (find_in_vector(v_sources,sso) != v_sources.end()) {
		std::cerr << ("SoundManager -> addSource: SoundSourceObject already exist\f");
		return;
	}

	v_sources.push_back(sso);
}

void SoundManager::addClip(std::shared_ptr <SoundClip> sc) {
	if (!sc) {
		throw std::runtime_error("SoundManager -> addClip: SoundClip not exist\f");
	}

	// check if exist a source with a given name
	if (find_in_vector(v_clips,sc) != v_clips.end()) {
		throw std::runtime_error("SoundManager -> addClip: SoundClip already exist\f");
	}

	v_clips.push_back(sc);
}


void SoundManager::addAudioToClip(std::shared_ptr <SoundClip> sc, const char* uri) {
	if (!sc) {
		throw std::runtime_error("SoundManager -> addAudioToClip: Shared pointer SoundClip sc is nullptr\f");
	}
	
	auto clip_find = find_in_vector(v_clips, sc);
	// check if exist a clip with a given name
	if (clip_find == v_clips.end()) {
		throw std::runtime_error("SoundManager -> addAudioToClip: SoundClip not already exist\f");
	}
	
	sc->addAudio(uri);
}



void SoundManager::connectClipWithSource(std::shared_ptr<SoundClip> sc, std::shared_ptr <SoundSourceObject> sso) {
	if (!sso) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject not exist\f");
	}

	if (!sc) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundClip not exist\f");
	}

	auto source_find = find_in_vector(v_sources, sso);
	auto clip_find = find_in_vector(v_clips, sc);

	//@TODO Maybe intead of an exception we should create a Clip or SourceObjecy

	// check if exist a clip with a given name
	if (clip_find == v_clips.end()) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundClip not already exist\f");
	}

	// check if exist a source with a given name
	if (source_find == v_sources.end()) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject not already exist in v_sources. Please add to map (sm.addSource(sso))\f");
	}

	sso->addBuffer(sc);
}



void SoundManager::playSound(std::shared_ptr <SoundSourceObject> sso){
	//std::lock_guard<std::mutex> lock(sound_manager_mutex);

	if (!sso) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject not exist\f");
	}
	

	auto source_find = find_in_vector(v_sources, sso);

	// check if exist a source with a given name
	if (source_find == v_sources.end()) {
		throw std::runtime_error("SoundManager -> playSound: SoundSourceObject not already exist in v_sources. Please add to map (sm.addSource(sso))\f");
	}

	sso->playSound();
}



void SoundManager::stopSound(std::shared_ptr <SoundSourceObject> sso) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);
	if (!sso) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject not exist\f");
	}

	auto source_find = find_in_vector(v_sources, sso);
	// check if exist a source with a given name
	if (source_find == v_sources.end()) {
		throw std::runtime_error("SoundManager -> stopSound: SoundSourceObject not already exist in ums_sources. Please add to map (sm.addSource(sso))\f");
	}
	sso->stopSound();
}

void SoundManager::pauseSound(std::shared_ptr <SoundSourceObject> sso) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);
	if (!sso) {
		throw std::runtime_error("SoundManager -> connectClipWithSource: SoundSourceObject not exist\f");
	}

	auto source_find = find_in_vector(v_sources, sso);
	// check if exist a source with a given name
	if (source_find == v_sources.end()) {
		throw std::runtime_error("SoundManager -> stopSound: SoundSourceObject not already exist in ums_sources. Please add to map (sm.addSource(sso))\f");
	}
	sso->pauseSound();
}

ALCcontext* SoundManager::getContext(){
	return this->p_ALCContext;
}

//ALCdevice* SoundManager::getDevice()
//{
//	return this->p_ALCDevice();
//}

template <typename T>
void removeExpired(std::vector<std::weak_ptr<T>>& vec) {
	vec.erase(std::remove_if(vec.begin(),vec.end(),
		[](const std::weak_ptr<T>& wp) {
			return wp.expired();
		}),vec.end());
}

void SoundManager::cleanupVectors() {
	removeExpired(v_sources);
	removeExpired(v_clips);
}