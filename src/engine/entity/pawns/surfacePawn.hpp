#pragma once
#include "../pawn.hpp"
#include "shared/math.hpp"

class SurfacePawn : public Pawn {
	glm::vec2 scale;
	glm::vec2 position;
	float rotation;

	glm::mat3x4 result;

	/*
	 * Translates Pawn by specified value
	 */
	void addPosition(glm::vec2 new_position);

	/*
     * Sets absolute position of a pawn in 2D space
     */
	void setPosition(glm::vec2 new_position);

	/*
     * Returns absolute position of a pawn in 2D space
     */
	glm::vec2 getPosition() const;


	/**
     * Sets absolute rotation of a pawn in 2D space in radians
     */
	void setRotation(float new_rotation);

	/**
	 * Rotates object to the left by given amount of radians
	 */
	void rotateLeft(float new_rotation);

	/**
     * Rotates object to the right by given amount of radians
     */
	void rotateRight(float new_rotation);

	/**
     * Returns absolute rotation of a pawn in 2D space in radians
     */
	float getRotation() const;

	/**
	 * Sets absolute scale of a pawn in 2D space
	 */
	void setScale(glm::vec2 new_scale);

	/**
     * Returns absolute scale of a pawn in 2D space
     */
	glm::vec2 getScale() const;

	/**
     * Returns affine transform matrix of an object. (Its rotation scale and position combined!)
     */
	glm::mat3x4 getMatrix() const;

	/**
     * Returns facing direction, normalised
     */
	glm::vec2 getForwardVector();

	/**
     * Returns facing direction, normalised
     */
	glm::vec3 getForwardVector3D(); 
};