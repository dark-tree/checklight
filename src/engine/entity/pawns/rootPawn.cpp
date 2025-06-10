#include "rootPawn.hpp"
#include "../../pawnTree.hpp"

/*
 * RootPawn 
 */

void RootPawn::setTree(PawnTree* tree) {
	tr = tree;
}

RootPawn::RootPawn() : Pawn() {
	name = "Root";
	tr = nullptr;
}

bool RootPawn::isRoot() {
	return true;
}

PawnTree* RootPawn::getTree() {
	return tr;
}
