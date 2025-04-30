#include "matrixAnimation.hpp"
#include "engine/entity/context.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"

MatrixAnimation::MatrixAnimation(SpatialPawn* s) : GameComponent(s){
	animationSpeed = 1;
	old_percentage = 0;
	percentage = 0;
	type = NONE;
}

MatrixAnimation::MatrixAnimation(SpatialPawn* s, MatrixAnimation::AnimationType type) : GameComponent(s){
	animationSpeed = 1;
	old_percentage = 0;
	percentage = 0;
	this->type = type;
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
	old_percentage = percentage;
	percentage += TICK_DURATION * animationSpeed;
	if(percentage > 1) percentage -= 1;
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
	return InputResult::PASS;
}

void MatrixAnimation::rotation() {
	auto rotation = (float)(percentage*M_PI*2);
	setRotation(glm::rotate(glm::identity<glm::quat>(),rotation,glm::vec3 (1,1,1)));
}

void MatrixAnimation::translation() {
	float v_1 = percentage > 0.5 ? 1 - percentage : percentage;
	float v_2 = old_percentage > 0.5 ? 1 - old_percentage : old_percentage;


	glm::vec3 position = glm::vec3(v_1,v_1,v_1) * -10.0f;
	glm::vec3 old_position = glm::vec3(v_2,v_2,v_2) * -10.0f;
	glm::vec3 posDiff = old_position - position;
	setPosition(getPosition()+posDiff);
}

void MatrixAnimation::shape() {

}

