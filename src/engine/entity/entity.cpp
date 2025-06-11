#include "entity.hpp"

/*
 * Entity
 */

Entity::Entity() {
	id = id_number++;
	active = true;
	to_remove = false;
}

Entity::~Entity() {
}

uint32_t Entity::getEntityID() const {
	return id;
}

void Entity::setActive(const bool value) {
	active = value;
}

void Entity::debugDraw(ImmediateRenderer& renderer) {
}
