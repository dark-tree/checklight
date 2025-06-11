#include "manager.hpp"

#include "shared/logger.hpp"

// ================================== PRIVATE ==================================

SoundManager::SoundManager(){
	p_ALCDevice = alcOpenDevice(nullptr);
	if (!p_ALCDevice) {
		FAULT("Failed to open OpenAL device");
		return;
	}

	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);
	if (!p_ALCContext || !alcMakeContextCurrent(p_ALCContext)) {
		alcCloseDevice(p_ALCDevice);
		FAULT("Failed to load OpenAL context");
		return;
	}

	out::info("Sound system ready!");
}

SoundManager::~SoundManager(){
	// free device and context
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(p_ALCContext);
	alcCloseDevice(p_ALCDevice);
}

void SoundManager::checkIfExistInVector(const std::string& error_text, std::vector<std::weak_ptr<SoundSourceObject>>& vector, const std::shared_ptr <SoundSourceObject>& obj) {
	auto source_find = findInVector(vector, obj);
	if (source_find == vector.end()) {
		out::error("%s\n", error_text.c_str());
	}
}

// ================================== PUBLIC ==================================

void SoundManager::addSource(const std::shared_ptr <SoundSourceObject>& sso){
	assert(sso);

	removeExpired(v_sources);

	if (findInVector(v_sources, sso) != v_sources.end()) {
		out::warn("There was an attempt to add a already preset source!");
		return;
	}

	v_sources.push_back(sso);
}

void SoundManager::addClip(const std::shared_ptr <SoundClip>& sc) {
	assert(sc);

	if (findInVector(v_clips,sc) != v_clips.end()) {
		out::warn("There was an attempt to add a already preset clip!");
		return;
	}

	v_clips.push_back(sc);
}

void SoundManager::removeClip(const std::shared_ptr<SoundClip>& sc) {
	removeFromVector(v_clips, sc);
}

void SoundManager::addGroup(const std::shared_ptr <SoundGroup>& sg) {
	assert(sg);

	removeExpired(v_groups);

	if (findInVector(v_groups, sg) != v_groups.end()) {
		out::warn("There was an attempt to add a already preset group!");
		return;
	}

	v_groups.push_back(sg);
}

void SoundManager::loadAudioToClip(const std::shared_ptr<SoundClip>& sc, const char* path) {
	assert(sc);
	sc->loadAudio(path);
}

void SoundManager::connectClipWithSource(const std::shared_ptr<SoundClip>& sc, const std::shared_ptr<SoundSourceObject>& sso) {
	assert(sso.get());
	assert(sc);

#if ENGINE_DEBUG
	checkIfExistInVector("SoundSourceObject isn't added yet! Please add it with SoundManager::addSource(sso)", v_sources, sso);
#endif // ENGINE_DEBUG

	sso->addBuffer(sc);
}

std::shared_ptr<SoundClip> SoundManager::createSoundClipAndLoadAudio(const char* path) {
	auto sc = std::make_shared<SoundClip>();
	addClip(sc);
	loadAudioToClip(sc, path);
	return sc;
}

std::shared_ptr<SoundClip> SoundManager::createSoundClipAndAddToSourceObject(const char* path,const std::shared_ptr<SoundSourceObject>& sso) {
	auto sc = std::make_shared<SoundClip>();
	addClip(sc);
	loadAudioToClip(sc, path);
	connectClipWithSource(sc, sso);
	return sc;
}

void SoundManager::playSound(const std::weak_ptr<SoundSourceObject>& sso){

#if ENGINE_DEBUG
	checkIfExistInVector("SoundSourceObject isn't added yet! Please add it with SoundManager::addSource(sso)", v_sources, sso.lock());
#endif // ENGINE_DEBUG

	if (auto sptr = sso.lock()) {
		sptr->playSound();
	} else {
		out::warn("Can't play expired sound!");
	}

}

void SoundManager::stopSound(const std::shared_ptr<SoundSourceObject>& sso) {
	assert(sso);

#if ENGINE_DEBUG
	checkIfExistInVector("SoundSourceObject isn't added yet! Please add it with SoundManager::addSource(sso)", v_sources, sso);
#endif // ENGINE_DEBUG

	sso->stopSound();
}

void SoundManager::pauseSound(const std::shared_ptr <SoundSourceObject>& sso) {
	assert(sso);

#if ENGINE_DEBUG
	checkIfExistInVector("SoundSourceObject isn't added yet! Please add it with SoundManager::addSource(sso)", v_sources, sso);
#endif // ENGINE_DEBUG

	sso->pauseSound();
}

void SoundManager::cleanupVectors() {
	removeExpired(v_sources);
	removeExpired(v_groups);
}