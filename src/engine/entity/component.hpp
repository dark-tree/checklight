#pragma once
#include "entity.hpp"

struct Context;
struct FixedContext;
class Pawn;

class Component : public Entity {
protected:
	Pawn* parent;

	/**
	 * All the things that happens on basic update of the engine (intervals between basic updates can vary)
	 */
	virtual void onUpdate(Context c) = 0;
	/**
     * All the things that happens on fixed update of the engine (fixed intervals between updates, updates with the same frequency as physics)
     */
	virtual void onFixedUpdate(FixedContext c) = 0;
	virtual bool checkValidity(const Pawn& p) = 0;
public:
	Component();

	/**
	 * TODO make it into a virtual function, that returns const char* lub std::string_view, potentially remove name form entity???? check if that breaks sth in pawn 
	 */
	std::string getComponentName() const;

	friend class Pawn;
};