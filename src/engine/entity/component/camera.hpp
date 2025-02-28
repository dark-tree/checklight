#pragma once
#include "../component.hpp"
#include "../pawns/spatialPawn.hpp"

class Camera : public Component {
protected:
	glm::fvec3 position;
	glm::fvec3 direction;

	virtual void onUpdate(Context c);

	virtual void onFixedUpdate(FixedContext c);

public:
	glm::fvec3 getPosition();

	glm::fvec3 getFactingDirection();
};