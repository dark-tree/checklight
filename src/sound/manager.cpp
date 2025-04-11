#include "manager.hpp"


// ================================== PRIVATE ==================================


SoundManager::SoundManager(){
	p_ALCDevice = alcOpenDevice(nullptr);
	if (!p_ALCDevice) {
		std::cerr << ("OpenAL -> init: Failed to open device\f");  //throw exception
		return;
	}

	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);
	if (!p_ALCContext || !alcMakeContextCurrent(p_ALCContext)) {
		alcCloseDevice(p_ALCDevice);
		std::cerr << ("OpenAL -> init: Failed to load context on the device\f");  //throw exception
		return;
	}

	printf("OpenAL initialized successfully.\n");
}

SoundManager::~SoundManager(){
	// delete vectors
	v_sources.clear();
	v_clips.clear();
	v_groups.clear();

	// free device and context
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(p_ALCContext);
	alcCloseDevice(p_ALCDevice);
}


// ================================== PUBLIC ==================================


void SoundManager::addSource(std::shared_ptr <SoundSourceObject> sso){
	if (!sso) {
		std::cerr<<("SoundManager -> addSource: SoundSourceObject not exist\f");
		return;
	}

	removeExpired(v_sources);
	// check if exist a source with a given name
	if (findInVector(v_sources,sso) != v_sources.end()) {
		std::cerr << ("SoundManager -> addSource: SoundSourceObject already exist\f");
		return;
	}

	v_sources.push_back(sso);
}

void SoundManager::removeSource(std::weak_ptr<SoundSourceObject> sso) {
	sso.lock()->~SoundSourceObject();
	removeExpired(v_sources);
}

void SoundManager::addClip(std::shared_ptr <SoundClip> sc) {
	if (!sc) {
		std::cerr << ("SoundManager -> addClip: SoundClip not exist\f");
		return;
	}

	// check if exist a source with a given name
	if (findInVector(v_clips,sc) != v_clips.end()) {
		std::cerr << ("SoundManager -> addClip: SoundClip already exist\f");
		return;
	}

	v_clips.push_back(sc);
}

void SoundManager::removeClip(std::shared_ptr<SoundClip> sc) {
	removeFromVector(v_clips, sc);
	if (sc) {
		sc->~SoundClip();
	}
	else {
		std::cerr << ("SoundManager -> removeClip: SoundClip not exist\f");
		return;
	}
}

void SoundManager::addGroup(std::shared_ptr <SoundGroup> sg) {
	if (!sg) {
		std::cerr << ("SoundManager -> addGroup: SoundGroup not exist\f");
		return;
	}

	removeExpired(v_groups);
	// check if exist a source with a given name
	if (findInVector(v_groups, sg) != v_groups.end()) {
		std::cerr << ("SoundManager -> addGroup: SoundGroup already exist\f");
		return;
	}
	v_groups.push_back(sg);
}

void SoundManager::removeGroup(std::weak_ptr<SoundGroup> sc) {
	sc.lock()->~SoundGroup();
	removeExpired(v_groups);
}

void SoundManager::addAudioToClip(std::shared_ptr <SoundClip> sc, const char* uri) {
	if (!sc) {
		std::cerr << ("SoundManager -> addAudioToClip: Shared pointer SoundClip sc is nullptr\f");
		return;
	}
	
	auto clip_find = findInVector(v_clips, sc);
	// check if exist a clip with a given name
	if (clip_find == v_clips.end()) {
		std::cerr << ("SoundManager -> addAudioToClip: SoundClip not already exist\f");
		return;
	}
	
	sc->addAudio(uri);
}


void SoundManager::connectClipWithSource(std::shared_ptr<SoundClip> sc, std::shared_ptr <SoundSourceObject> sso) {
	if (!sso) {
		std::cerr << ("SoundManager -> connectClipWithSource: SoundSourceObject not exist\f");
		return;
	}

	if (!sc) {
		std::cerr << ("SoundManager -> connectClipWithSource: SoundClip not exist\f");
		return;
	}

	removeExpired(v_sources);
	auto source_find = findInVector(v_sources, sso);
	auto clip_find = findInVector(v_clips, sc);

	//@TODO Maybe intead of an exception we should create a Clip or SourceObjecy

	// check if exist a clip with a given name
	if (clip_find == v_clips.end()) {
		std::cerr << ("SoundManager -> connectClipWithSource: SoundClip not already exist\f");
		return;
	}

	// check if exist a source with a given name
	if (source_find == v_sources.end()) {
		std::cerr << ("SoundManager -> connectClipWithSource: SoundSourceObject not already exist in v_sources. Please add to map (sm.addSource(sso))\f");
		return;
	}

	sso->addBuffer(sc);
}

std::shared_ptr<SoundClip> SoundManager::createSoundClipAndAddAudio(const char* url) {
	auto sc = std::make_shared<SoundClip>();
	addClip(sc);
	addAudioToClip(sc, url);
	return sc;
}

std::shared_ptr<SoundClip> SoundManager::createSoundClipAndAddToSourceObject(const char* url, std::shared_ptr<SoundSourceObject> sso) {
	auto sc = std::make_shared<SoundClip>();
	addClip(sc);
	addAudioToClip(sc, url);
	connectClipWithSource(sc, sso);
	return sc;
}

void SoundManager::playSound(std::weak_ptr <SoundSourceObject> sso){
	//std::lock_guard<std::mutex> lock(sound_manager_mutex);

	if (sso.expired()) {
		std::cerr << ("SoundManager -> playSound: SoundSourceObject not exist\f");
		return;
	}
	
	removeExpired(v_sources);
	auto source_find = findInVector(v_sources, sso);

	// check if exist a source with a given name
	if (source_find == v_sources.end()) {
		std::cerr << ("SoundManager -> playSound: SoundSourceObject not already exist in v_sources. Please add to map (sm.addSource(sso))\f");
		return;
	}

	sso.lock()->playSound();
}


void SoundManager::stopSound(std::shared_ptr <SoundSourceObject> sso) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);
	if (!sso) {
		std::cerr << ("SoundManager -> stopSound: SoundSourceObject not exist\f");
		return;
	}

	removeExpired(v_sources);
	auto source_find = findInVector(v_sources, sso);
	// check if exist a source with a given name
	if (source_find == v_sources.end()) {
		std::cerr << ("SoundManager -> stopSound: SoundSourceObject not already exist in ums_sources. Please add to map (sm.addSource(sso))\f");
		return;
	}
	sso->stopSound();
}


void SoundManager::pauseSound(std::shared_ptr <SoundSourceObject> sso) {
	std::lock_guard<std::mutex> lock(sound_manager_mutex);
	if (!sso) {
		std::cerr << ("SoundManager -> pauseSound: SoundSourceObject not exist\f");
		return;
	}

	removeExpired(v_sources);
	auto source_find = findInVector(v_sources, sso);
	// check if exist a source with a given name
	if (source_find == v_sources.end()) {
		std::cerr << ("SoundManager -> pauseSound: SoundSourceObject not already exist in ums_sources. Please add to map (sm.addSource(sso))\f");
		return;
	}
	sso->pauseSound();
}

void SoundManager::cleanupVectors() {
	removeExpired(v_sources);
	removeExpired(v_groups);
}