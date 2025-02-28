#pragma once
#include "gameObject.hpp"
#include "entity.hpp"


GameObject::GameObject() : Entity() {
	active = true;
}

bool GameObject::isActive() const {
	return active;
}