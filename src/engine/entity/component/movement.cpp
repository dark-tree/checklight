//
// Created by tymon on 13/11/2025.
//
#include "movement.hpp"
#include "physics.hpp"

MovementComponent::MovementComponent(SpatialPawn *s) : GameComponent(s) {
    direction = {1, 0, 0};
    pressed_right = false;
    pressed_left = false;
    pressed_backwards = false;
    pressed_forward = false;
    pressed_space = false;

    acceleration = 10.0;
    max_speed = 1.0;
    jumpTrigger = 4.1;

    physicsComponent = std::static_pointer_cast<PhysicsComponent>(getSpatialParent()->getComponents()[0]);

}

void MovementComponent::onUpdate(Context c) {
    float speed_multiplier = c.delta * acceleration;

    glm::vec3 speed = physicsComponent->getVelocity();

    if(pressed_forward) {
        speed += direction * speed_multiplier;
    }
    if(pressed_backwards) {
        speed -= direction * speed_multiplier;
    }
    if(pressed_left) {
        speed -= normalize(glm::cross(direction, math::UP)) * speed_multiplier;
    }
    if(pressed_right) {
        speed += normalize(glm::cross(direction, math::UP)) * speed_multiplier;
    }

    if (glm::length(glm::vec3(speed.x, 0, speed.z)) > max_speed)
    {
        speed = glm::normalize(glm::vec3(speed.x, 0, speed.z)) * (float)max_speed + glm::vec3(0, speed.y, 0);
    }

    if (pressed_space && jumpTrigger >= getSpatialParent()->getPosition().y)
    {
        speed += glm::vec3{0, 6, 0};
        pressed_space = false;
    }

    //printf("%f, %f, %f \n", speed.x, speed.y, speed.z);

    physicsComponent->setVelocity(speed);
}

void MovementComponent::onFixedUpdate(FixedContext c) {

}

void MovementComponent::onConnected() {

}

InputResult MovementComponent::onEvent(const InputEvent &event) {
    if(const auto* key_event = event.as<KeyboardEvent>()) {
        //forward
        if(key_event->wasPressed(GLFW_KEY_W)) {
            pressed_forward = true;
        }
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
        else if(key_event->wasPressed(GLFW_KEY_SPACE)) pressed_space = true;
            //down
        //printf("skibidiusz");
    }
    return InputResult::PASS;
}

void MovementComponent::setMaxSpeed(double speed) {
    this->max_speed = speed;
}

void MovementComponent::setAcceleration(double speed) {
    this->acceleration = speed;
}

void MovementComponent::setDirection(glm::vec3 dir) {
    this->direction = glm::normalize(glm::vec3{dir.x, 0, dir.z});
}


