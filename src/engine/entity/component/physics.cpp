#include "physics.hpp"


PhysicsComponent::PhysicsComponent(SpatialPawn* sp, Collider collider, bool is_static, Material material,
                                   glm::vec3 gravity_scale) : GameComponent(sp) {
	this->is_static = is_static;
	this->gravity_scale = gravity_scale;
	this->material = material;
	this->collider = collider;
	this->mass = calculateMass();
	this->initMass = true;
	this->collider.setInertiaTensor(glm::mat3x3(mass));
}

PhysicsComponent::PhysicsComponent(SpatialPawn* sp): GameComponent(sp) {
	this->is_static = false;
	this->gravity_scale = glm::vec3(1, 1, 1);
	this->material = Material(0.4, 0.5, 1);
	this->collider = Collider::getCube();
	this->mass = calculateMass();
	this->initMass = true;
	this->collider.setInertiaTensor(glm::mat3x3(mass));
}


void PhysicsComponent::onFixedUpdate(FixedContext c) {
	initMass = false;
}

void PhysicsComponent::setCollider(const Collider& c) {
	this->collider = c;
}

void PhysicsComponent::setGravityScale(glm::vec3 scale) {
	this->gravity_scale = scale;
}

void PhysicsComponent::setMass(float mass) {
	this->mass = mass;
}

void PhysicsComponent::setSurface(Surface surface) {
	this->material = surface;
}

void PhysicsComponent::setStatic(bool is_static) {
	this->is_static = is_static;
}

glm::vec3 PhysicsComponent::getGravityScale() const {
	return gravity_scale;
}

Surface PhysicsComponent::getMaterial() const {
	return material;
}

bool PhysicsComponent::isStatic() const {
	return is_static;
}

Collider& PhysicsComponent::getCollider() {
	return collider;
}

std::shared_ptr<RenderObject> PhysicsComponent::getRenderObject() {
	return render_object;
}

glm::vec3 PhysicsComponent::furthestPointInDirection(glm::vec3 direction) {
	const glm::vec3 position = getPosition();
	const glm::quat rotation = getRotation();

	double dot_result = -INFINITY;
	glm::vec3 returnal{0, 0, 0};
	for (glm::vec3 vertex: collider.getVertices()) {
		glm::vec3 rotated = glm::rotate(rotation, vertex);
		double dr = glm::dot(rotated, direction);
		if (dr > dot_result) {
			dot_result = dr;
			returnal = rotated;
		}
	}
	return returnal + position;
}

float PhysicsComponent::calculateMass() {
	return material.density * collider.getVolume();
}

float PhysicsComponent::getMass() {
	if (!initMass) mass = calculateMass();
	return mass;
}

void PhysicsComponent::onUpdate(Context c) {
}

void PhysicsComponent::onConnected() {
}

InputResult PhysicsComponent::onEvent(const InputEvent& event) { return InputResult::PASS; }

void PhysicsComponent::remove() {
	GameComponent::remove();
}

PhysicsComponent::~PhysicsComponent() {
}
