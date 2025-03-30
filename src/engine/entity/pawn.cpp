#include "pawn.hpp"

#include <utility>
#include "pawns/rootPawn.hpp"
#include "../pawnTree.hpp"
#include "../board.hpp"
#include "shared/logger.hpp"


/*
 * PawnState
 */

std::string PawnState::to_str(PawnState::State p) {
	switch (p) {
		case State::NEW: return "new";
		case State::LOCAL: return "local";
		case State::TRACKED: return "tracked";
		case State::UNPINNED: return "unpinned";
		case State::REMOVED: return "removed";
		case State::UNLISTED: return "unlisted";
		case State::SINGLE: return "single";
		default: return "unknown";
	}
}

/*
 * Pawn
 */

void Pawn::onUpdate() {
	std::shared_ptr<Pawn> p = shared_from_this();
	Context cntx(0.01f, p);
	for (const std::shared_ptr<Component>& c : components) {
		c->onUpdate(cntx);
	}
}

void Pawn::onFixedUpdate() {
	FixedContext cntx;
	for (const std::shared_ptr<Component>& c : components) {
		c->onFixedUpdate(cntx);
	}
}

Pawn::Pawn() : Entity() {
	is_mounted_to_board = false;
	unregistered_child_added = false;
	to_remove = false;
	board = nullptr;
	pawn_state = PawnState::NEW;
}


void Pawn::addComponent(std::shared_ptr<Component>& c) {
	if (c->checkValidity(*this)) {
		c->parent = this;
		components.push_back(std::move(c));
	}
}

void Pawn::setBoard(Board* s) {
	board = s;
}


template <typename T>
typename std::enable_if<std::is_base_of<Component, T>::value, void>::type
Pawn::createComponent() {
	std::shared_ptr<Component> c = std::make_unique<T>();
	if (c->checkValidity(*this)) {
		c->parent = this;
		components.push_back(std::move(c));
	}
}

bool Pawn::isRooted() {
	if (!root_pawn.expired()) {
		return true;
	}

	bool rooted = false;
	std::shared_ptr<Pawn> recursive_parent = parent.lock();

	// recursive_parent will be nullptr if parent expired
	while (recursive_parent) {
		if (recursive_parent->isRoot()/*std::dynamic_pointer_cast<RootPawn>(recursive_parent.lock())*/) {
			rooted = true;
			root_pawn = std::static_pointer_cast<RootPawn>(recursive_parent);
		}
		recursive_parent = recursive_parent->getParent();
	}
	return rooted;
}

std::string Pawn::getPawnName() const {
	return std::remove_reference_t<decltype(*this)>::class_name;
}

std::shared_ptr<Pawn> Pawn::getParent() const {
	if(parent.expired()) return nullptr;
	else return parent.lock();
}

Board* Pawn::getScene() {
	if(is_mounted_to_board) return board;
	else return nullptr;
}

std::vector<std::shared_ptr<Pawn>> Pawn::getChildren() {
	return children;
}

void Pawn::addChild(std::shared_ptr<Pawn> new_child) {
	//std::shared_ptr<Pawn> parent = new_child->getParent();
	//new_child->parent = std:: this; TODO
	new_child->parent = shared_from_this();

	if(pawn_state == PawnState::NEW){
		pawn_state = PawnState::LOCAL;
	}
	children.push_back(new_child); //cannot be std move (it would destroy the argument)
	if (isRooted()) {
		if (root_pawn.expired()) {
			FAULT("Root Pawn shouldn't have expired after isRooted() call!");
		}

		PawnTree* pt = root_pawn.lock()->getTree();
		if (pt == nullptr) {
			FAULT("Pawn Tree shouldn't be null, all Root Pawns should be children of one Pawn Tree!");
		}

		pt->mountPawn(new_child);
	}
	else{
		if(!isRoot()){
			unregistered_child_added = true; //only when the change is not instantly commited to a data structure
		}
	}
}

std::string Pawn::getName() {
	return name;
}

void Pawn::setName(std::string new_name) {
	name = new_name;
}

bool Pawn::isRoot() {
	return false;
}

bool Pawn::unregisteredChildAdded() const {
	return unregistered_child_added;
}

bool Pawn::isMountedToBoard() const{
	return is_mounted_to_board;
}

std::string Pawn::toString() const{
	if(name != "") return name;
	else return "Unnamed Pawn";
}

std::string Pawn::toStringVerbose() const{
	std::string result;
	result +=
			"{ id: " +
			std::to_string(id) +
			", name: \"" +
			Pawn::toString() +
			"\", children: " +
			std::to_string(children.size()) +
			", update: " +
			std::to_string(unregistered_child_added) +
			", remove: " +
			std::to_string(to_remove) +
			", state: " +
			PawnState::to_str(pawn_state) +
			", components: " +
			std::to_string(components.size()) +
			", parent: " +
			(getParent() != nullptr ? "1" : "0") +
			"}";
	return result;
}

std::shared_ptr<RootPawn> Pawn::getRoot() {
	if(isRooted()) return root_pawn.lock();
	else return nullptr;
}


