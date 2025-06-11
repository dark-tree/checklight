#pragma once
#include "game.hpp"
#include "../pawns/spatialPawn.hpp"

class Camera : public GameComponent {
protected:
	glm::vec3 position{};
	glm::vec3 direction{};

	glm::vec2 mouse_position{};
	glm::vec2 mouse_position_old{};

	glm::vec2 virtual_mouse_position{};

	double speed;
	double speed_modifier;

	bool mouse_init;
	bool mouse_captured;

	bool pressed_left,
			pressed_right,
			pressed_up,
			pressed_down,
			pressed_forward,
			pressed_backwards,
			mouse_move;

	void onUpdate(Context c) override;

	void onFixedUpdate(FixedContext c) override;

	void onConnected() override;

	InputResult onEvent(const InputEvent& event) override;

public:
	Camera(SpatialPawn* s);

	glm::vec3 getCamPosition();

	glm::vec3 getCamFacing();

	double getSpeed();

	void setSpeed(double speed);
};
