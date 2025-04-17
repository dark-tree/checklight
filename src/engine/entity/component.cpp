#include "component.hpp"
#include "pawn.hpp"
#include "context.hpp"

/*
 * Component
 */

Component::Component() : Entity() {
	parent = nullptr;
}

std::string Component::getComponentName() const {
	return std::remove_reference_t<decltype(*this)>::class_name;
}

std::string Component::toString() {
	return "{ id: " + std::to_string(id) + " type: \"" + getComponentName() + "\" }";
}


