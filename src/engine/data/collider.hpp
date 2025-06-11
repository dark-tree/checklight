#pragma once
#include "external.hpp"
#include "render/api/mesh.hpp"

class Collider {
protected:
	std::vector<glm::vec3> vertices;
	std::vector<glm::ivec3> triangles;
	glm::vec3 center_of_mass;

	float sphere_collider_radius;
	glm::mat3x3 inertia_tensor;

	float volume;

	/// Finds the volume of an object, will be called if volumen is null
	float findVolume();

	/// Used to calculate center of mass of an object. Assumes uniform mass distribution
	glm::vec3 findCenterOfMass();

	void calculateSphereColliderRadius();

	glm::mat3x3 findInertiaTensor();

public:
	Collider();

	static Collider getCube();

	std::vector<glm::vec3> getVertices();

	void setVertices(const std::vector<glm::vec3>& vertices);

	std::vector<glm::ivec3> getTriangles();

	void setTriangles(const std::vector<glm::ivec3>& triangles);

	void LoadFromModel(std::shared_ptr<RenderMesh>);

	void setAutoCenter();

	/** Sets the center of mass of the object
	 * @warning Incorrectly setting the center of mass can result in horrible force application miscalculations
	 */
	void setCenterOfMass(glm::vec3 center_of_mass);

	/** Sets the center of mass of the object
	 * @warning Incorrectly setting the center of mass can result in horrible force application miscalculations
	 */
	void setCenterOfMass(float x, float y, float z);

	/** Sets the inertia tensor of the object
	 * @warning Incorrectly setting the inertia tensor can result in horrible force application miscalculations
	 */
	void setInertiaTensor(const glm::mat3x3& inertia_tensor);

	/// Gets the center of mass of the object
	glm::vec3 getCenterOfMass() const;

	/// Gets the inertia tensor of the object
	glm::mat3x3 getInertiaTensor() const;

	/// Gets the radius of the sphere collider
	float getSphereColliderRadius() const;

	/// Returns the volume of an object
	float getVolume() const;
};
