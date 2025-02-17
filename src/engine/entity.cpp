#include "entity.hpp"

/*
 * Entity
 */

Entity::Entity() {
	entityID = idNumber++;
}

uint32_t Entity::getEntityID() const {
	return entityID;
}

uint32_t Entity::idNumber = 0;