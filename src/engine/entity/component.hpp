#pragma once
#include "entity.hpp"
#include "input/input.hpp"

struct Context;
struct FixedContext;
class Pawn;

class Component : public Entity, public InputListener {
protected:
	///Pawn that owns this component
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
	InputResult onEvent(const InputEvent& event) override = 0;
public:
	Component();

	/**
	 * TODO make it into a virtual function, that returns const char* lub std::string_view, potentially remove name form entity???? check if that breaks sth in pawn
	 */
	std::string getComponentName() const;
	void onAdded();

	friend class Pawn;
};