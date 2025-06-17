#pragma once
#include "../component.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"

class SpatialPawn;

class GameComponent : public OwnedComponent<SpatialPawn> {
public:
	GameComponent() = delete;

	GameComponent(SpatialPawn* t);

	SpatialPawn* getSpatialParent() const;

	glm::vec3 getPosition() const;

	glm::quat getRotation() const;

	void setPosition(const glm::vec3 position) const;

	void setRotation(const glm::quat rotation) const;

	glm::vec3 getForwardVector() const;

	glm::vec3 getVelocity() const;

	glm::vec3 getAngularVelocity() const;

	void setVelocity(glm::vec3 velocity);

	void setAngularVelocity(glm::vec3 angularVelocity);
};
