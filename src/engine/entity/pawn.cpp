#include "pawn.hpp"
#include "pawns/rootPawn.hpp"
#include "../pawnTree.hpp"
#include "../board.hpp"

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
	is_mounted_to_scene = false;
	changed_pawn_struture = false;
}


void Pawn::addComponent(std::shared_ptr<Component>& c) {
	if (c->checkValidity(*this)) {
		c->parent = this;
		components.push_back(std::move(c));
	}
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
	if (!rootPawn.expired()) {
		return true;
	}

	bool rooted = false;
	std::shared_ptr<Pawn> recursive_parent = parent.lock();

	// recursive_parent will be nullptr if parent expired
	while (recursive_parent) {
		if (recursive_parent->isRoot()/*std::dynamic_pointer_cast<RootPawn>(recursive_parent.lock())*/) {
			rooted = true;
			rootPawn = std::static_pointer_cast<RootPawn>(recursive_parent);
		}
		recursive_parent = recursive_parent->getParent().lock();
	}
	return rooted;
}

std::string Pawn::getPawnName() const {
	return std::remove_reference_t<decltype(*this)>::class_name;
}

std::weak_ptr<Pawn> Pawn::getParent() {
	return parent;
}

std::vector<std::shared_ptr<Pawn>> Pawn::getChildren() {
	return children;
}

void Pawn::addChild(std::shared_ptr<Pawn> new_child) {
	changed_pawn_struture = true;
	children.push_back(new_child); //cannot be std move (it would destroy the argument)
	if (isRooted()) {
		if (rootPawn.expired()) {
			throw std::runtime_error("It shouldnt be expired after isRooted() function, sth went wrong");
		}
		else {
			PawnTree* pt = rootPawn.lock()->getTree();
			if (pt == nullptr) {
				throw std::runtime_error("It shouldnt be null here, all rootPawns should be a children of one pawnTree");
			}
			else {
				pt->mountPawn(new_child);
			}
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

bool Pawn::isStructureChanged() {
	return changed_pawn_struture;
}