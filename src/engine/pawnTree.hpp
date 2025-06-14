#pragma once
#include "entity/component/physics.hpp"
#include "entity/pawns/rootPawn.hpp"

class PawnTree {
	friend Board;

protected:
	std::shared_ptr<RootPawn> root;

	std::unordered_multimap<std::string, std::shared_ptr<Pawn>> name_map;
	std::unordered_multimap<uint32_t, std::shared_ptr<Pawn>> id_map;
	std::set<std::shared_ptr<PhysicsComponent>> physics_components_to_update;

	/**
	 * performs standard game update on all the tree elements, triggered by updateTree() function
	 */
	void updateTreeRecursion(std::shared_ptr<Pawn> pawn_to_update, double delta, std::mutex& mtx);

	/**
	 * performs standard game update on all the tree elements, triggered by fixedUpdateTree() function
	 */
	void fixedUpdateTreeRecursion(std::shared_ptr<Pawn> pawn_to_fixed_update);

	/**
	 * returns part of a pawn tree in a string format, triggered by print() function
	 */
	std::string recursiveString(const std::shared_ptr<Pawn>& p, int depth, bool verbose);

	/**
	 * manages verbose/non-verbose printing
	 */
	std::string printStart(bool verbose);

	/**
	 * removes a pawn from hashmaps, returns true if operation was successful
	 */
	bool removeFromMaps(const std::string& name, uint32_t id);

	/**
	 * adds pawn to a hashmaps
	 */
	void addPawnToHash(const std::string& p_name, uint32_t p_id, const std::shared_ptr<Pawn>& pawn);

public:
	PawnTree();

	/**
	 * finds a pawn by name
	 */
	std::shared_ptr<Pawn> findByName(const std::string& name);

	/**
	 * finds a pawn by id
	 */
	std::shared_ptr<Pawn> findByID(uint32_t id);

	/**
	 * finds all pawns with given name
	 */
	std::vector<std::shared_ptr<Pawn>> findAllByName(const std::string& name);

	/**
	 * finds all pawns with given id
	 */
	std::vector<std::shared_ptr<Pawn>> findAllByID(uint32_t id);

	/**
	 * returns size of entry in map with given key (name)
	 */
	size_t nameHitSize(const std::string& name) const;

	/**
	 * returns size of entry in map with given key (id)
	 */
	size_t idHitSize(uint32_t id) const;

	/**
	 * adds a pawn to RootPawn
	 */
	void addToRoot(const std::shared_ptr<Pawn>& pawn);

	/**
	 * updates/inserts a pawn to a PawnTree
	 */
	void mountPawn(const std::shared_ptr<Pawn>& pawn);

	/**
	 * updates children of given pawn in a PawnTree
	 */
	void updatePawnsChildren(const std::shared_ptr<Pawn>& pawn);

	/**
	 * returns a RootPawn of given tree
	 */
	std::shared_ptr<RootPawn> getRoot();

	/**
	 * performs standard game update on all the tree elements
	 */
	void updateTree(double delta, std::mutex& mtx);

	/**
	 * performs fixed game update on all the tree elements
	 */
	void fixedUpdateTree();

	/**
	 * returns whole pawn tree in string format
	 */
	std::string toString();

	/**
	 * returns whole pawn tree in string format
	 */
	std::string toStringVerbose();

	/**
	 * registers physicsComponent to be included in physics update
	 */
	void registerPhysicsComponent(const std::shared_ptr<PhysicsComponent>& physics_component);

	/**
	 * removes physicsComponent from registry that stores components to update in physics update
	 */
	void removePhysicsComponent(const std::shared_ptr<PhysicsComponent>& physics_component);

	/**
	 * returns
	 */
	std::set<std::shared_ptr<PhysicsComponent>> getPhysicsComponents();
};
