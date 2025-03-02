#include "camera.hpp"

/*
 * Camera
 */

Camera::Camera() : GameComponent() {
	position = { 0,0,0 };
	direction = { 1,0,0 };
}

void Camera::onUpdate(Context c) {
    auto* s = (SpatialPawn*)parent;
    position = s->getPosition();
    direction = s->getForwardVector();
}

void Camera::onFixedUpdate(FixedContext c) {

}

glm::fvec3 Camera::getCamPosition() {
	return position;
}

glm::fvec3 Camera::getCamFacing() {
	return direction;
}

InputResult Camera::onEvent(const InputEvent &event) {
    SpatialPawn* p = getSpatialParent();
    glm::vec3 pos = p->getPosition();
    glm::quat rot = p->getRotation();


    if (const auto* key_event = event.as<KeyboardEvent>()) {
        //forward
        if(key_event->wasPressed(GLFW_KEY_W)){
            pos += p->getForwardVector();
        }
        //left
        if(key_event->wasPressed(GLFW_KEY_A)){
            //TODO
        }
        //back
        if(key_event->wasPressed(GLFW_KEY_S)){
            pos -= p->getForwardVector();
        }
        //right
        if(key_event->wasPressed(GLFW_KEY_D)){
            //TODO
        }
        //down
        if(key_event->wasPressed(GLFW_KEY_Q) || key_event->wasPressed(GLFW_KEY_LEFT_SHIFT)){
            pos += glm::vec3{0,-1.0,0};
        }
        //up
        if(key_event->wasPressed(GLFW_KEY_E) || key_event->wasPressed(GLFW_KEY_SPACE)){
            pos += glm::vec3{0,1.0,0};
        }
    }

    p->setPosition(pos);
    p->setRotation(rot);

    return InputResult::PASS;
}
