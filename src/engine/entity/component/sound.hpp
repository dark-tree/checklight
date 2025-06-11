#pragma once
#include "game.hpp"
#include "sound/sound.hpp"


class SoundComponent : public GameComponent {

	std::shared_ptr<SoundSourceObject> sound_source_object;
public:
	SoundComponent(SpatialPawn* t, const std::string& path);

	~SoundComponent() override;

protected:
	void onUpdate(Context c) override;

	void onFixedUpdate(FixedContext c) override;

	InputResult onEvent(const InputEvent& event) override;

	void onConnected() override;
};
