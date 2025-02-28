#pragma once
#include "entity.hpp"
#include "component.hpp"
#include "input/input.hpp"
#include "context.hpp"

class PawnTree;
class Scene;
class RootPawn;

class Pawn : public Entity, public std::enable_shared_from_this<Pawn> {
protected:
	friend class PawnTree;
	std::vector<std::shared_ptr<Pawn>> children;
	std::weak_ptr<Pawn> parent;
	bool is_mounted_to_scene;
	bool changed_pawn_struture;
	std::weak_ptr<Scene> scene;
	std::string name;
	std::weak_ptr<RootPawn> rootPawn;

	std::vector<std::shared_ptr<Component>> components; //TODO unique ptr ???

	/**
     * All the things that happens on basic update of the engine (intervals between basic updates can vary)
     */
	virtual void onUpdate();

	/**
	 * All the things that happens on fixed update of the engine (fixed intervals between updates, updates with the same frequency as physics)
	 */
	virtual void onFixedUpdate();

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
	 * Returns a pawns parent
	 */
	std::weak_ptr<Pawn> getParent();

	/**
     * Returns all of the pawns children
     */
	std::vector<std::shared_ptr<Pawn>> getChildren();

	/**
	 * Adds a new children to a pawn
	 */
	void addChild(std::shared_ptr<Pawn> new_child);

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
     * returns true if there were some changes to pawn tree (like new children added) that requires changes to PawnTree structure
     */
	bool isStructureChanged();
};