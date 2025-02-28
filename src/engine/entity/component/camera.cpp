#include "camera.hpp"

/*
 * Camera
 */

void Camera::onUpdate(Context c) {
	if (dynamic_cast<SpatialPawn*>(parent)) { //TODO delete?
		SpatialPawn* s = (SpatialPawn*)parent;
		position = s->getPosition();
		direction = s->getForwardVector();
	}
	else {
		throw std::runtime_error("Pawn does not inherit class \"Spatial Pawn\". ");
	}
}

void Camera::onFixedUpdate(FixedContext c) {

}

glm::fvec3 Camera::getPosition() {
	return position;
}

glm::fvec3 Camera::getFactingDirection() {
	return direction;
}
