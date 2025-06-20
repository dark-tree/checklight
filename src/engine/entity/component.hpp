#pragma once
#include "entity.hpp"
#include "input/input.hpp"
#include "../trait.hpp"
#include "render/render.hpp"

struct Context;
struct FixedContext;
class Pawn;

class Component : public Entity, public InputListener {
protected:
	friend Pawn;
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

	InputResult onEvent(const InputEvent& event) override = 0;


	/**
	 * Executes when it's added to a pawn
	 */
	virtual void onConnected() = 0;

public:
	Component(Pawn* p) : Entity() {
		parent = p;
	}

	/**
	 * TODO make it into a virtual function, that returns const char* lub std::string_view, potentially remove name form entity???? check if that breaks sth in pawn
	 */
	std::string getComponentName() const;

	virtual void remove();

	std::string toString();
};

template<DerivedTrait<Pawn> T>
struct OwnedComponent : Component {
	OwnedComponent() = delete;

	OwnedComponent(T* t) : Component(t) {
	}
};
