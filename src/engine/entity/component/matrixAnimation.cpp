#include "matrixAnimation.hpp"
#include "engine/entity/context.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"

MatrixAnimation::MatrixAnimation() {
	animationSpeed = 1;
	percentage = 0;
	type = NONE;
}

void MatrixAnimation::setAnimation(MatrixAnimation::AnimationType newType) {
	type = newType;
}

MatrixAnimation::AnimationType MatrixAnimation::getAnimation() const{
	return type;
}

void MatrixAnimation::onUpdate(Context c) {

}

void MatrixAnimation::onFixedUpdate(FixedContext c) {
	switch (type) {
		case NONE:
			break;
		case ROTATE:
			rotation();
			break;
		case TRANSLATE:
			translation();
			break;
		case SHAPE:
			shape();
			break;
		default:
			break;
	}
}

void MatrixAnimation::onConnected() {

}

InputResult MatrixAnimation::onEvent(const InputEvent &event) {
	return Component::onEvent(event);
}

void MatrixAnimation::rotation() {

}

void MatrixAnimation::translation() {

}

void MatrixAnimation::shape() {

}
