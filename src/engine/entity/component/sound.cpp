#include "sound.hpp"

SoundComponent::SoundComponent(SpatialPawn* t,const std::string& path) : GameComponent(t) {
	SoundManager& sound_manager = SoundManager::getInstance();

	sound_source_object = std::make_shared<SoundSourceObject>();
	sound_manager.addSource(sound_source_object);
	sound_manager.createSoundClipAndAddToSourceObject(path.c_str(), sound_source_object);
}

SoundComponent::~SoundComponent() {
}

void SoundComponent::onUpdate(Context c) {
	SoundManager::getInstance().playSound(sound_source_object);
}

void SoundComponent::onFixedUpdate(FixedContext c) {
	sound_source_object->setPosition(getPosition());
}

InputResult SoundComponent::onEvent(const InputEvent& event) {
	return InputResult::PASS;
}

void SoundComponent::onConnected() {

}
