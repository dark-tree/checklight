//
// Created by tymon on 13/11/2025.
//
#include "game.hpp"
#include "../pawns/spatialPawn.hpp"

class MovementComponent : public GameComponent {
protected:
    double acceleration;
    double max_speed;
    double jumpTrigger;

    std::shared_ptr<PhysicsComponent> physicsComponent;

    glm::vec3 direction{};

    bool pressed_left,
            pressed_right,
            pressed_forward,
            pressed_backwards,
            pressed_space;

    void onUpdate(Context c) override;

    void onFixedUpdate(FixedContext c) override;

    void onConnected() override;

    InputResult onEvent(const InputEvent& event) override;

public:

    void setMaxSpeed(double speed);

    void setAcceleration(double speed);

    void setDirection(glm::vec3);

    MovementComponent(SpatialPawn* s);
};
