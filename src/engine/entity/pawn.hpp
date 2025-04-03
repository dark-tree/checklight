#pragma once
#include "entity.hpp"
#include "component.hpp"
#include "input/input.hpp"
#include "context.hpp"

class PawnTree;
class Board;
class RootPawn;


namespace PawnState {
	enum State{
		NEW,
		LOCAL,
		TRACKED,
		UNPINNED,
		REMOVED,
		UNLISTED,
		SINGLE
	};

	/**
	 * string representation of the State enum
	 */
	std::string to_str(State p);

	/**
	 * Select state both for child and parent after adding a child to a parent (avoid using manually)
	 */
	bool convert(Pawn* new_child,Pawn* new_parent);
}


class Pawn : public Entity, public std::enable_shared_from_this<Pawn> {
protected:
	friend class PawnTree;
	friend class Board;
	friend bool PawnState::convert(Pawn* new_child,Pawn* new_parent);
	std::vector<std::shared_ptr<Pawn>> children; //TODO updating this to make sure its up to date when removing children
	std::weak_ptr<Pawn> parent;
	bool to_remove;
	PawnState::State pawn_state;

	bool is_mounted_to_board;

	/// checks if there is a complex structure of pawns and uses that for mounting a child into data structures
	bool unregistered_child_added;

	///checks if the pawn is saved to hashmap
	bool is_tracked_on_hash;

	Board* board;
	std::string name;
	std::weak_ptr<RootPawn> root_pawn;

	std::vector<std::shared_ptr<Component>> components; //TODO unique ptr ???

	/**
	 * All the things that happens on basic update of the engine (intervals between basic updates can vary)
	 */
	virtual void onUpdate();

	/**
	 * All the things that happens on fixed update of the engine (fixed intervals between updates, updates with the same frequency as physics)
	 */
	virtual void onFixedUpdate();

	void setBoard(Board* s);

	/**
	 * Removes all the children of a pawn
	 */
	void propagateRemove();

	/**
	 * Similar to remove function, but there is no need to check if the pawn is root
	 */
	bool safeRemove();

	/**
	 * Sets tracked_on_hash variable
	 */
	void setTracked(bool v);


	void removeComponents();
public:

	Pawn();

	/**
	 * Adds a new component to a pawn
	 */
	void addComponent(std::shared_ptr<Component>& c);

	/**
	 * TODO will be removed
	 */
	template <typename T>
	typename std::enable_if<std::is_base_of<Component, T>::value, void>::type
		createComponent();

	/**
	 * Checks if a pawn belongs to a Scene (contains a RootPawn in its parent chain)
	 */
	bool isRooted();

	/**
	 * TODO make it into a virtual function, that returns const char* lub std::string_view, potentially remove name form entity???? check if that breaks sth in pawn
	 */
	std::string getPawnName() const;

	/**
	 * Returns a pawns parent or nullptr if empty
	 */
	std::shared_ptr<Pawn> getParent() const;

	/**
	 * Returns the board of which it is a part of, or null if there is no such scene
	 */
	Board* getScene();

	/**
	 * Returns all of the pawns children
	 */
	std::vector<std::shared_ptr<Pawn>>& getChildren();

	/**
	 * Adds a new children to a pawn
	 */
	void addChild(const std::shared_ptr<Pawn>& new_child);

	/**
	 * Returns a name that is specific to that instance of a pawn
	 */
	std::string getName();

	/**
	 * Sets name of this instance of a pawn
	 */
	void setName(std::string new_name);


	/**
	 * Returns true if its a root of an pawn tree (therefore of RootPawn type)
	 */
	virtual bool isRoot();

	/**
	 * returns true if there were some additive changes to pawn tree (like new children added) that requires changes to PawnTree structure
	 */
	bool unregisteredChildAdded() const;

	/**
	 * returns true if there were some subtractive changes to pawn tree (like removed a children) that requires changes to PawnTree structure
	 */
	//bool childToBeRemoved() const;

	/**
	 * returns true if the pawn is a part of a board
	 */
	bool isMountedToBoard() const;

	/**
	 * returns small amount of information about the pawn ina a string format, currently only a name [for full description use toStringVerbose]
	 */
	std::string toString() const;

	/**
	 * returns information about the pawn in a string string
	 */
	std::string toStringVerbose() const;

	/**
	 * returns root pawn or nullptr if root pawn doesn't exists
	 */
	std::shared_ptr<RootPawn> getRoot();

	/**
	 * returns information about the state of pawn in regards to the way its stored and its visibility
	 */
	PawnState::State getState() const;

	/**
	 * mark a Pawn and all its children as "Removed", unpin it from Pawn Tree
	 */
	bool remove();
};