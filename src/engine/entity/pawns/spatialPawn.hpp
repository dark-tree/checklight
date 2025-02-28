#pragma once
#include "../pawn.hpp"
#include "shared/math.hpp"

class SpatialPawn : public Pawn {
protected:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::mat3x4 result;

public:

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
	glm::mat3x4 getMatrix() const;

	/**
     * Returns facing direction, TODO check if this is normalised
     */
	glm::vec3 getForwardVector();
};