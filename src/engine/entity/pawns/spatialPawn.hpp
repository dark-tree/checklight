#pragma once
#include "../pawn.hpp"
#include "shared/math.hpp"

class SpatialPawn : public Pawn {
protected:
	glm::vec3 velocity;
	glm::vec3 angular_velocity;

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::mat4x3 affineTransformMatrix;

public:
	SpatialPawn();

	COMPONENT_BIND_POINT

	/**
	 * Translates Pawn by specified value
	 */
	void addPosition(glm::vec3 new_position);

	/**
	 * Sets absolute position of a pawn in 3D space
	 */
	void setPosition(glm::vec3 new_position);

	/**
	 * Returns absolute position of a pawn in 3D space
	 */
	glm::vec3 getPosition() const;

	/**
	 * Sets velocity in 3D space
	 */
	void setVelocity(glm::vec3 velocity);

	/**
	 * Returns velocity in 3D space
	 */
	glm::vec3 getVelocity();

	/**
	 * Sets angular velocity in 3D space
	 */
	void setAngularVelocity(glm::vec3 velocity);

	/**
	 * Returns angular velocity in 3D space
	 */
	glm::vec3 getAngularVelocity();

	/**
	 * Sets absolute rotation of a pawn in 3D space
	 */
	void setRotation(glm::quat new_rotation);

	/**
	 * Returns absolute rotation of a pawn in 3D space
	 */
	glm::quat getRotation() const;

	/**
	 * Sets absolute scale of a pawn in 3D space
	 */
	void setScale(glm::vec3 new_scale);

	/**
	 * Sets absolute scale of a pawn in 3D space
	 */
	glm::vec3 getScale() const;

	/**
	 * Returns affine transform matrix of an object. (Its rotation scale and position combined!)
	 */
	glm::mat4x3 getMatrix() const;

	/**
	 * Returns facing direction
	 */
	glm::vec3 getForwardVector() const;
};
