#pragma once
#include "game.hpp"
#include "render/render.hpp"
#include "engine/data/models.hpp"
#include "engine/data/collider.hpp"
#include "engine/data/material.hpp"

class PhysicsComponent : public GameComponent {
protected:
	std::shared_ptr<RenderObject> render_object;
	glm::vec3 velocity;
	glm::vec3 angular_velocity;

	Collider collider;

	bool is_static;
	glm::vec3 gravity_scale;

	float mass;
	bool initMass;

	Material material;

	float calculateMass();

public:
	//PhysicsComponent(SpatialPawn *sp, Collider collider, bool is_static, Material material, glm::vec3 gravity_scale);
	PhysicsComponent(SpatialPawn *sp);

	void onFixedUpdate(FixedContext c) override;

	/// Sets the velocity using a glm::vec3
	void setVelocity(glm::vec3 velocity);

	/// Sets the velocity using individual float components
	void setVelocity(float x, float y, float z);

	/// Sets the angular velocity using a glm::vec3
	void setAngularVelocity(glm::vec3 angular_velocity);

	/// Sets the angular velocity using individual float components
	void setAngularVelocity(float x, float y, float z);

	/// Sets the gravity scale for the object
	void setGravityScale(glm::vec3 scale);

	/// Sets the mass of the object
	void setMass(float mass);

	/// Sets the surface properties of the object
	void setSurface(Surface surface);

	/// Sets whether the object is static (non-movable)
	void setStatic(bool is_static);


	/// Gets the current velocity
	glm::vec3 getVelocity() const;

	/// Gets the current angular velocity
	glm::vec3 getAngularVelocity() const;

	/// Gets the gravity scale
	glm::vec3 getGravityScale() const;

	/// Gets the surface properties
	Surface getMaterial() const;

	/// Returns true if the object is static
	bool isStatic() const;

	/// Gets the collider
	Collider getCollider();

	/// Gets the associated render object
	std::shared_ptr<RenderObject> getRenderObject();

	/// Returns the furthest point from object origin in a given direction in relation to the world space
	glm::vec3 furthestPointInDirection(glm::vec3 direction);

	/// Returns the mass of an object
	float getMass(); //TODO use mass inside of engine

	void onUpdate(Context c) override;

	void onConnected() override;

	InputResult onEvent(const InputEvent &event) override;

	void remove() override;

	~PhysicsComponent() override;
};
