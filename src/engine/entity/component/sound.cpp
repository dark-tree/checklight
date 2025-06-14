#include "sound.hpp"

SoundComponent::SoundComponent(SpatialPawn* t,const std::string& path) : GameComponent(t) {
	SoundManager& sound_manager = SoundManager::getInstance();

	sound_source_object = std::make_shared<SoundSourceObject>();
	sound_manager.addSource(sound_source_object);
	sound_manager.createSoundClipAndAddToSourceObject(path.c_str(), sound_source_object);
	//sound_source_object->setReferenceDistance(10.f);
}

SoundComponent::~SoundComponent() {
}

void SoundComponent::onUpdate(Context c) {
	SoundManager::getInstance().playSound(sound_source_object);
}

void SoundComponent::onFixedUpdate(FixedContext c) {
	sound_source_object->setPosition(getPosition());
	sound_source_object->setVelocity(getVelocity());
}

InputResult SoundComponent::onEvent(const InputEvent& event) {
	return InputResult::PASS;
}

void SoundComponent::onConnected() {

}

void SoundComponent::debugDraw(ImmediateRenderer& renderer) {
	const glm::vec3 position = getPosition();
	renderer.setBillboardMode(BillboardMode::TWO_AXIS);
	renderer.setSprite(default_file_name);
	renderer.drawRect3D(position.x,position.y,position.z,1.0f,1.0f);
}
