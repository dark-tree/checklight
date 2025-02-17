#pragma once
#include "entity.hpp"

class GameObject : public Entity{
protected:
	bool active;
public:
	GameObject();
	virtual void update() = 0;

	bool isActive() const;
};