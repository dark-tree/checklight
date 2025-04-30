#include "manager.hpp"

// ================================== PRIVATE ==================================

SoundManager::SoundManager(){
	p_ALCDevice = alcOpenDevice(nullptr);
	if (!p_ALCDevice) {
		std::cerr << ("OpenAL -> init: Failed to open device\n");  //throw exception
		return;
	}

	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);
	if (!p_ALCContext || !alcMakeContextCurrent(p_ALCContext)) {
		alcCloseDevice(p_ALCDevice);
		std::cerr << ("OpenAL -> init: Failed to load context on the device\n");  //throw exception
		return;
	}

	printf("OpenAL initialized successfully.\n");
}

SoundManager::~SoundManager(){
	// free device and context
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(p_ALCContext);
	alcCloseDevice(p_ALCDevice);
}

void SoundManager::checkIfExistInVector(const std::string error_text, std::vector<std::weak_ptr<SoundSourceObject>>& vector, const std::shared_ptr <SoundSourceObject>& obj) {
	auto source_find = findInVector(vector, obj);
	if (source_find == vector.end()) {
		std::cerr << error_text;
		return;
	}
}

// ================================== PUBLIC ==================================

void SoundManager::addSource(const std::shared_ptr <SoundSourceObject>& sso){
	if (!sso) {
		std::cerr<<("SoundManager -> addSource: SoundSourceObject not exist\n");
		return;
	}

	removeExpired(v_sources);
	// check if exist a source with a given name
	if (findInVector(v_sources,sso) != v_sources.end()) {
		std::cerr << ("SoundManager -> addSource: SoundSourceObject already exist\n");
		return;
	}

	v_sources.push_back(sso);
}

void SoundManager::addClip(const std::shared_ptr <SoundClip>& sc) {
	if (!sc) {
		std::cerr << ("SoundManager -> addClip: SoundClip not exist\n");
		return;
	}

	// check if exist a source with a given name
	if (findInVector(v_clips,sc) != v_clips.end()) {
		std::cerr << ("SoundManager -> addClip: SoundClip already exist\n");
		return;
	}

	v_clips.push_back(sc);
}

void SoundManager::removeClip(const std::shared_ptr<SoundClip>& sc) {
	removeFromVector(v_clips, sc);
}

void SoundManager::addGroup(const std::shared_ptr <SoundGroup>& sg) {
	if (!sg) {
		std::cerr << ("SoundManager -> addGroup: SoundGroup not exist\n");
		return;
	}

	removeExpired(v_groups);
	// check if exist a source with a given name
	if (findInVector(v_groups, sg) != v_groups.end()) {
		std::cerr << ("SoundManager -> addGroup: SoundGroup already exist\n");
		return;
	}

	v_groups.push_back(sg);
}

void SoundManager::loadAudioToClip(const std::shared_ptr <SoundClip>& sc, const char* path) {
	if (!sc) {
		std::cerr << ("SoundManager -> addAudioToClip: Shared pointer SoundClip sc is nullptr\n");
		return;
	}

	sc->loadAudio(path);
}

void SoundManager::connectClipWithSource(const std::shared_ptr<SoundClip>& sc,const std::shared_ptr <SoundSourceObject>& sso) {
	if (!sso) {
		std::cerr << ("SoundManager -> connectClipWithSource: SoundSourceObject not exist\n");
		return;
	}

	if (!sc) {
		std::cerr << ("SoundManager -> connectClipWithSource: SoundClip not exist\n");
		return;
	}

#if ENGINE_DEBUG
	checkIfExistInVector("SoundManager -> connectClipWithSource: SoundSourceObject not already exist in v_sources. Please add to map (sm.addSource(sso))\n", v_sources, sso);
#endif // ENGINE_DEBUG

	sso->addBuffer(sc);
}

std::shared_ptr<SoundClip>& SoundManager::createSoundClipAndLoadAudio(const char* path) {
	auto sc = std::make_shared<SoundClip>();
	addClip(sc);
	loadAudioToClip(sc, path);
	return sc;
}

std::shared_ptr<SoundClip>& SoundManager::createSoundClipAndAddToSourceObject(const char* path,const std::shared_ptr<SoundSourceObject>& sso) {
	auto sc = std::make_shared<SoundClip>();
	addClip(sc);
	loadAudioToClip(sc, path);
	connectClipWithSource(sc, sso);
	return sc;
}

void SoundManager::playSound(const std::weak_ptr <SoundSourceObject>& sso){

	if (sso.expired()) {
		std::cerr << ("SoundManager -> playSound: SoundSourceObject not exist\n");
		return;
	}
	
#if ENGINE_DEBUG
	checkIfExistInVector("SoundManager -> playSound: SoundSourceObject not already exist in v_sources. Please add to map (sm.addSource(sso))\n", v_sources, sso.lock());
#endif // ENGINE_DEBUG

	sso.lock()->playSound();
}

void SoundManager::stopSound(const std::shared_ptr <SoundSourceObject>& sso) {
	if (!sso) {
		std::cerr << ("SoundManager -> stopSound: SoundSourceObject not exist\n");
		return;
	}

#if ENGINE_DEBUG
	checkIfExistInVector("SoundManager -> stopSound: SoundSourceObject not already exist in ums_sources. Please add to map (sm.addSource(sso))\n", v_sources, sso);
#endif // ENGINE_DEBUG

	sso->stopSound();
}

void SoundManager::pauseSound(const std::shared_ptr <SoundSourceObject>& sso) {
	if (!sso) {
		std::cerr << ("SoundManager -> pauseSound: SoundSourceObject not exist\n");
		return;
	}

#if ENGINE_DEBUG
	checkIfExistInVector("SoundManager -> pauseSound: SoundSourceObject not already exist in ums_sources. Please add to map (sm.addSource(sso))\n", v_sources, sso);
#endif // ENGINE_DEBUG

	sso->pauseSound();
}

void SoundManager::cleanupVectors() {
	removeExpired(v_sources);
	removeExpired(v_groups);
}