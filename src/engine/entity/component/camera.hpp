#pragma once
#include "../component.hpp"
#include "../pawns/spatialPawn.hpp"

class Camera : public Component {
protected:
	glm::vec3 position;
	glm::vec3 direction;

	virtual void onUpdate(Context c);

	virtual void onFixedUpdate(FixedContext c);

	Camera();

public:
	glm::vec3 getPosition();

	glm::vec3 getFactingDirection();
};