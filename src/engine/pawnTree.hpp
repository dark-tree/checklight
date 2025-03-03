#pragma once
#include "entity/pawns/rootPawn.hpp"

class PawnTree {
protected:

	RootPawn root;

	std::unordered_multimap<std::string, std::shared_ptr<Pawn>> nameMap;
	std::unordered_multimap<uint32_t, std::shared_ptr<Pawn>> idMap;

	/**
	 * performs standard game update on all the tree elements, triggered by updateTree() function
	 */
	void updateTreeRecursion(std::shared_ptr<Pawn> pawn_to_update);

	/**
	 * performs standard game update on all the tree elements, triggered by fixedUpdateTree() function
	 */
	void fixedUpdareTreeRecursion(std::shared_ptr<Pawn> pawn_to_fixed_update);

	/**
	 * returns part of a pawn tree in a string format, triggered by print() function
	 */
	std::string recursivePrint(std::shared_ptr<Pawn> p, int depth);

public:

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
	size_t nameHitSize(const std::string& name);

	/**
	 * returns size of entry in map with given key (id)
	 */
	size_t idHitSize(uint32_t id);

	/**
	 * adds a pawn to RootPawn
	 */
	void addToRoot(std::shared_ptr<Pawn> pawn);

	/**
	 * updates/inserts a pawn to a PawnTree
	 */
	void mountPawn(std::shared_ptr<Pawn> pawn); //TODO moze dac update/insert

	/**
	 * updates children of given pawn in a PawnTree
	 */
	void updatePawnsChildren(std::shared_ptr<Pawn>& pawn);

	/**
	 * returns a RootPawn of given tree
	 */
	RootPawn getRoot();

	/**
	 * performs standard game update on all the tree elements
	 */
	void updateTree();

	/**
	 * performs fixed game update on all the tree elements
	 */
	void fixedUpdateTree();

	/**
	 * returns whole pawn tree in string format
	 */
	std::string print();
};