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

bool PawnState::convert(Pawn* new_child,Pawn* new_parent) {
	PawnState::State childState = new_child->getState();
	switch (new_parent->getState()){
		case NEW:
			switch (childState) {
				case NEW:       //parent - NEW, child - NEW
					new_child->pawn_state = LOCAL;
					break;
				case LOCAL:     //parent - NEW, child - LOCAL
					new_child->pawn_state = LOCAL;
					break;
				case TRACKED:   //parent - NEW, child - TRACKED
					new_child->pawn_state = UNLISTED;
					break;
				case UNPINNED:  //parent - NEW, child - UNPINNED
					new_child->pawn_state = UNLISTED;
					break;
				case REMOVED:   //parent - NEW, child - REMOVED
					FAULT("Cant add child, the child is marked as REMOVED");
					break;
				case UNLISTED:  //parent - NEW, child - UNLISTED
					new_child->pawn_state = UNLISTED;
					break;
				case SINGLE:    //parent - NEW, child - SINGLE
					new_child->pawn_state = LOCAL;
					break;
				default:        //parent - NEW, child - UNKNOWN
					FAULT("Unknown child pawn state");
			}
			new_parent->pawn_state = LOCAL;
			break;
		case LOCAL:
			switch (childState) {
				case NEW:      // parent - LOCAL, child - NEW
					new_child->pawn_state = LOCAL;
					break;
				case LOCAL:    // parent - LOCAL, child - LOCAL
					new_child->pawn_state = LOCAL;
					break;
				case TRACKED:  // parent - LOCAL, child - TRACKED
					new_child->pawn_state = UNLISTED;
					break;
				case UNPINNED: // parent - LOCAL, child - UNPINNED
					new_child->pawn_state = UNLISTED;
					break;
				case REMOVED:  // parent - LOCAL, child - REMOVED
					FAULT("Cant add child, the child is marked as REMOVED");
					break;
				case UNLISTED: // parent - LOCAL, child - UNLISTED
					new_child->pawn_state = UNLISTED;
					break;
				case SINGLE:   // parent - LOCAL, child - SINGLE
					new_child->pawn_state = LOCAL;
					break;
				default:       // parent - LOCAL, child - UNKNOWN
					FAULT("Unknown child pawn state");
			}
			break;
		case TRACKED:
			switch (childState) {
				case NEW:      // parent - TRACKED, child - NEW
					new_child->pawn_state = TRACKED;
					break;
				case LOCAL:    // parent - TRACKED, child - LOCAL
					new_child->pawn_state = TRACKED;
					break;
				case TRACKED:  // parent - TRACKED, child - TRACKED
					new_child->pawn_state = TRACKED;
					break;
				case UNPINNED: // parent - TRACKED, child - UNPINNED
					new_child->pawn_state = TRACKED;
					break;
				case REMOVED:  // parent - TRACKED, child - REMOVED
					FAULT("Cant add child, the child is marked as REMOVED");
					break;
				case UNLISTED: // parent - TRACKED, child - UNLISTED
					new_child->pawn_state = TRACKED;
					break;
				case SINGLE:   // parent - TRACKED, child - SINGLE
					new_child->pawn_state = TRACKED;
					break;
				default:       // parent - TRACKED, child - UNKNOWN
					FAULT("Unknown child pawn state");
			}
			break;
		case UNPINNED:
			switch (childState) {
				case NEW:      // parent - UNPINNED, child - NEW
					new_child->pawn_state = UNPINNED;
					break;
				case LOCAL:    // parent - UNPINNED, child - LOCAL
					new_child->pawn_state = UNPINNED;
					break;
				case TRACKED:  // parent - UNPINNED, child - TRACKED
					new_child->pawn_state = UNPINNED;
					break;
				case UNPINNED: // parent - UNPINNED, child - UNPINNED
					new_child->pawn_state = UNPINNED;
					break;
				case REMOVED:  // parent - UNPINNED, child - REMOVED
					FAULT("Cant add child, the child is marked as REMOVED");
					break;
				case UNLISTED: // parent - UNPINNED, child - UNLISTED
					new_child->pawn_state = UNPINNED;
					break;
				case SINGLE:   // parent - UNPINNED, child - SINGLE
					new_child->pawn_state = UNPINNED;
					break;
				default:       // parent - UNPINNED, child - UNKNOWN
					FAULT("Unknown child pawn state");
			}
			break;
		case REMOVED:
			FAULT("Cant add child, the parent is marked as REMOVED");
		case UNLISTED:
			switch (childState) {
				case NEW:      // parent - UNLISTED, child - NEW
					new_child->pawn_state = LOCAL;
					break;
				case LOCAL:    // parent - UNLISTED, child - LOCAL
					new_child->pawn_state = LOCAL;
					break;
				case TRACKED:  // parent - UNLISTED, child - TRACKED
					new_child->pawn_state = UNLISTED;
					break;
				case UNPINNED: // parent - UNLISTED, child - UNPINNED
					new_child->pawn_state = UNLISTED;
					break;
				case REMOVED:  // parent - UNLISTED, child - REMOVED
					FAULT("Cant add child, the child is marked as REMOVED");
					break;
				case UNLISTED: // parent - UNLISTED, child - UNLISTED
					new_child->pawn_state = UNLISTED;
					break;
				case SINGLE:   // parent - UNLISTED, child - SINGLE
					new_child->pawn_state = LOCAL;
					break;
				default:       // parent - UNLISTED, child - UNKNOWN
					FAULT("Unknown child pawn state");
			}
			break;
		case SINGLE:
			switch (childState) {
				case NEW:      // parent - SINGLE, child - NEW
					new_child->pawn_state = LOCAL;
					break;
				case LOCAL:    // parent - SINGLE, child - LOCAL
					new_child->pawn_state = LOCAL;
					break;
				case TRACKED:  // parent - SINGLE, child - TRACKED
					new_child->pawn_state = UNLISTED;
					break;
				case UNPINNED: // parent - SINGLE, child - UNPINNED
					new_child->pawn_state = UNLISTED;
					break;
				case REMOVED:  // parent - SINGLE, child - REMOVED
					FAULT("Cant add child, the child is marked as REMOVED");
					break;
				case UNLISTED: // parent - SINGLE, child - UNLISTED
					new_child->pawn_state = UNLISTED;
					break;
				case SINGLE:   // parent - SINGLE, child - SINGLE
					new_child->pawn_state = LOCAL;
					break;
				default:       // parent - SINGLE, child - UNKNOWN
					FAULT("Unknown child pawn state");
			}
			new_parent->pawn_state = LOCAL;
			break;
		default:               // parent - UNKNOWN child - ...
			FAULT("Unknown parent pawn state");
	}
	return true;
}

/*
 * Pawn
 */

void Pawn::onUpdate(double delta) {
	std::shared_ptr<Pawn> p = shared_from_this();
	Context cntx(delta, p);
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
	is_tracked_on_hash = false;
}


void Pawn::addComponent(std::shared_ptr<Component>& c) {
	if (c->checkValidity(*this)) {
		c->parent = this;
		c->onConnected();
		components.push_back(std::move(c));
	}
}

void Pawn::setBoard(Board* s) {
	board = s;
}

void Pawn::propagateRemove() {
	for(const std::shared_ptr<Pawn>& c : children){
		c->safeRemove();
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

std::vector<std::shared_ptr<Pawn>>& Pawn::getChildren() {
	return children;
}

void Pawn::addChild(const std::shared_ptr<Pawn>& new_child) {
	new_child->parent = shared_from_this();
	//converts state of child and this pawn based on their previous state
	PawnState::convert(new_child.get(), this);

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

std::shared_ptr<RootPawn> Pawn::getRoot(){
	if(isRooted()) return root_pawn.lock();
	else return nullptr;
}

PawnState::State Pawn::getState() const {
	return pawn_state;
}

bool Pawn::remove() {
	if(isRoot()){
		FAULT("Cant remove root pawn!");
	}
	if(!to_remove){
#ifdef ENGINE_DEBUG
		if(pawn_state == PawnState::REMOVED)
			FAULT("Pawn state is REMOVED before using remove() function on it");
#endif
		to_remove = true;

		//this needs to be before destroying children and setting pawn state, otherwise it will break
		propagateRemove();

		pawn_state = PawnState::REMOVED;

		board->queueRemove(shared_from_this());

		//removing components
		for(auto c : components){
			c->remove();
			board->queueRemove(c);
		}
		return true;
	}
	else{
#ifdef ENGINE_DEBUG
		for(const auto& c : components){
			if(c->to_remove) FAULT("all children should be removed in a pawn described as removed!");
		}
#endif
		out::warn("Trying to remove() the same pawn more than once!");
		return false;
	}
}

bool Pawn::safeRemove(){
#ifdef ENGINE_DEBUG
	if(isRoot()){
		FAULT("This shouldn't be root as its as this function should only be called for children of removed pawn");
	}
#endif
	if(to_remove){
#ifdef ENGINE_DEBUG
		if(pawn_state == PawnState::REMOVED)
			FAULT("Pawn state is REMOVED before using remove() function on it");
#endif
		to_remove = true;

		propagateRemove();

		board->queueRemove(shared_from_this());

		return true;
	}
	else{
		out::warn("While removing children of an object Engine tried to remove() the same pawn more than once!");
		return false;
	}
}

void Pawn::setTracked(bool v) {
	is_tracked_on_hash = v;
}

void Pawn::removeComponents() {
	for(const auto& c : components){
		c->setActive(false);
	}
}



