#include "camera.hpp"

/*
 * Camera
 */



Camera::Camera() : GameComponent() {
	position = { 0,0,0 };
	direction = { 1,0,0 };
	pressed_right = false;
	pressed_left = false;
	pressed_up = false;
	pressed_down = false;
	pressed_backwards = false;
	pressed_forward = false;
	mouse_move = false;
	mouse_captured = true;

	mouse_position = {0, 0};
	mouse_position_old = {0, 0};
	mouse_init = false;
}

void Camera::onUpdate(Context c) {
	float DELTA = 0.01f; //temporary

	glm::vec3 pos = getPosition();
	glm::quat rot = getRotation();
	glm::vec3 forward = getForwardVector();

	if(pressed_forward){
		pos += forward * DELTA;
	}
	if(pressed_backwards){
		pos -= forward * DELTA;
	}
	if(pressed_down){
		pos += glm::vec3 {0.0f,-1.0f,0.0f} * DELTA;
	}
	if(pressed_up){
		pos += glm::vec3 {0.0f,1.0f,0.0f} * DELTA;
	}
	if(pressed_left){
		pos -= glm::cross(forward, math::UP) * DELTA;
	}
	if(pressed_right){
		pos += glm::cross(forward, math::UP) * DELTA;
	}
	if(mouse_move){
		glm::vec2 mouse_difference = mouse_position - mouse_position_old;
		mouse_position_old = mouse_position;

		virtual_mouse_position += mouse_difference/400.0f;

		virtual_mouse_position.y = std::clamp(virtual_mouse_position.y, glm::radians(-89.0f), glm::radians(89.0f));

		float moveX = virtual_mouse_position.x;
		float moveY = virtual_mouse_position.y;

		glm::quat rotX;
		glm::quat rotY;

		rotX = glm::rotate(glm::identity<glm::quat>(),moveX,{0,-1.0,0});
		rotY = glm::rotate(glm::identity<glm::quat>(),moveY,{0.0,0,-1.0});

		rot = rotX * rotY;

		mouse_move = false;
	}

	setPosition(pos);
	setRotation(rot);

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
	if (const auto* key_event = event.as<KeyboardEvent>()) {
		//forward
		if(key_event->wasPressed(GLFW_KEY_W)) pressed_forward = true;
		else if(key_event->wasReleased(GLFW_KEY_W)) pressed_forward = false;
		//left
		else if(key_event->wasPressed(GLFW_KEY_A)) pressed_left = true;
		else if(key_event->wasReleased(GLFW_KEY_A)) pressed_left = false;
		//back
		else if(key_event->wasPressed(GLFW_KEY_S)) pressed_backwards = true;
		else if(key_event->wasReleased(GLFW_KEY_S)) pressed_backwards = false;
		//right
		else if(key_event->wasPressed(GLFW_KEY_D)) pressed_right = true;
		else if(key_event->wasReleased(GLFW_KEY_D)) pressed_right = false;
		//down
		else if(key_event->wasPressed(GLFW_KEY_Q) || key_event->wasPressed(GLFW_KEY_LEFT_SHIFT)) //TODO make two separate ifs?
		    pressed_down = true;
		else if(key_event->wasReleased(GLFW_KEY_Q) || key_event->wasReleased(GLFW_KEY_LEFT_SHIFT))
		    pressed_down = false;
		//up
		else if(key_event->wasPressed(GLFW_KEY_E) || key_event->wasPressed(GLFW_KEY_SPACE))
		    pressed_up = true;
		else if(key_event->wasReleased(GLFW_KEY_E) || key_event->wasReleased(GLFW_KEY_SPACE))
		    pressed_up = false;
		//uncapture
		else if(key_event->wasReleased(GLFW_KEY_ESCAPE)){
			mouse_captured = false;
			mouse_init = false;
		}
	}
	if (const auto* button_event = event.as<ButtonEvent>()) {
		if(button_event->wasPressed(GLFW_MOUSE_BUTTON_LEFT)){
			mouse_captured = true;
		}
	}
	if (const auto* mouse_event = event.as<MouseEvent>()) {
		if(mouse_captured){
			mouse_move = true;
			if(!mouse_init){
				mouse_position_old = mouse_event->getMouse();
				mouse_position = mouse_event->getMouse();
			}
			else{
				mouse_position = mouse_event->getMouse();
			}
			mouse_init = true;
			return InputResult::ACCEPT;
		}
	}
	if (const auto* frame_event = event.as<FrameEvent>()) {
		if(mouse_captured) frame_event->capture();
	}
	return InputResult::PASS;
}

void Camera::onConnected() {

}
