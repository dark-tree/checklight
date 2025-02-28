#pragma once
#include "external.hpp"

class Entity {
protected:
	static uint32_t idNumber;
	uint32_t entityID;
public:
	Entity();

	uint32_t getEntityID() const;
};
