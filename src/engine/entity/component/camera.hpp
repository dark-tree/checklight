#pragma once
#include "gameComponent.hpp"
#include "../pawns/spatialPawn.hpp"

class Camera : public GameComponent {
protected:
	glm::vec3 position{};
	glm::vec3 direction{};

	void onUpdate(Context c) override;

    void onFixedUpdate(FixedContext c) override;

    InputResult onEvent(const InputEvent& event) override;

public:
	glm::vec3 getCamPosition();

	glm::vec3 getCamFacing();

    Camera();
};