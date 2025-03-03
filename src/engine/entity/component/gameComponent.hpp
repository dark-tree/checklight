#pragma once
#include "../component.hpp"

class SpatialPawn;

class GameComponent : public Component { // TODO wymyslec lepsza nazwe
protected:

    bool checkValidity(const Pawn& p) override;

    SpatialPawn* getSpatialParent();

    glm::vec3 getPosition();

    glm::quat getRotation();

    void setPosition(glm::vec3 position);

    void setRotation(glm::quat rotation);

    glm::vec3 getForwardVector();
};