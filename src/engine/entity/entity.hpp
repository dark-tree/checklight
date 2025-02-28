#pragma once
#include "../../external.hpp"
;
class Entity {
protected:
	bool active;
	bool to_remove;
	inline static uint32_t id_number = 0;
	inline static std::string class_name = "base entity";
	uint32_t id;

public:
	Entity();

	uint32_t getEntityID() const;

	/*
     * if set true component will execute update and fixedUpdate functions
	 */ 
	void setActive(bool value);

	/*
	 * if set true component will be removed at the end of the update cycle
	 */
	void queueRemove(bool value);
};
