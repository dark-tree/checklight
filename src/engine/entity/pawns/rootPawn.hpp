#pragma once
#include "../pawn.hpp"

class PawnTree;

class RootPawn : public Pawn {

	friend class PawnTree;
	PawnTree* tr;

protected:

	/**
	 * Sets PawnTree that manages access to Pawn. PawnTree is an object that stores root pawn and maps for quick pawn and component lookups.
	 */
	void setTree(PawnTree* tree);

public:
	RootPawn();

	/**
     * Returns true!
     */
	bool isRoot() override;

	/**
     * Returns PawnTree that manages access to Pawn. PawnTree is an object that stores root pawn and maps for quick pawn and component lookups.
     */
	PawnTree* getTree();
};