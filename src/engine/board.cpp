#include "board.hpp"
#include "entity/pawns/spatialPawn.hpp"
#include "shared/logger.hpp"

/*
 * Board
 */

Board::Board() {
	SoundManager::getInstance(); //unfortunately sound system is kinda cooked, so I need to do this
	pawns_to_remove = new std::queue<std::shared_ptr<Pawn>>();
	pawns_to_remove_from_hashmap = new std::queue<std::shared_ptr<Pawn>>();
	components_to_remove = new std::queue<std::shared_ptr<Component>>();
}

void Board::queueRemove(const std::shared_ptr<Pawn>& p_to_remove) const {
#if ENGINE_DEBUG
	if (p_to_remove->to_remove && p_to_remove->getState() != PawnState::REMOVED) {
		FAULT("Pawn unprepared to be removed!");
	}
#endif
	if (p_to_remove->is_tracked_on_hash) {
		//pawn needs to be removed from hashmap
		pawns_to_remove->push(p_to_remove);
		pawns_to_remove_from_hashmap->push(p_to_remove);
	} else {
		pawns_to_remove->push(p_to_remove);
	}
}

void Board::queueRemove(const std::shared_ptr<Component>& pawns_to_remove) {
	components_to_remove->push(pawns_to_remove);
}

void Board::updateBoard(double delta, std::mutex& mtx) {
	pawns.updateTree(delta, mtx);
	SoundListener::setPosition(this->getCamPos());
	SoundListener::setOrientation(this->getCamForward(), {0.0f,1.0f,0.0f});
}


void Board::fixedUpdateBoard() {
	pawns.fixedUpdateTree();
}


void Board::addPawnToRoot(const std::shared_ptr<Pawn>& pawn) {
	pawns.addToRoot(pawn);
	pawn->setBoard(this);
}


std::shared_ptr<Pawn> Board::findPawnByID(const uint32_t id) {
	return pawns.findByID(id);
}


std::shared_ptr<Pawn> Board::findPawnByID(const uint32_t id, const size_t position) {
	std::vector query_result = pawns.findAllByID(id);
	if (query_result.size() > position) {
		return query_result[position];
	}

	FAULT("Trying to get nonexistent Pawn!");
}


std::shared_ptr<Pawn> Board::findPawnByID(const int32_t id) {
	if (id < 0) {
		FAULT("ID can't be negative!");
	}

	return pawns.findByID(static_cast<uint32_t>(id));
}


std::shared_ptr<Pawn> Board::findPawnByID(const int32_t id, const size_t position) {
	if (id < 0) {
		FAULT("ID can't be negative!");
	}

	std::vector query_result = pawns.findAllByID(static_cast<uint32_t>(id));
	if (query_result.size() > position) {
		return query_result[position];
	}

	FAULT("Trying to get nonexistent Pawn!");
}


std::vector<std::shared_ptr<Pawn>> Board::findPawnsByID(const int32_t id) {
	return pawns.findAllByID(id);
}


std::shared_ptr<Pawn> Board::findPawnByName(const std::string& name) {
	return pawns.findByName(name);
}


std::shared_ptr<Pawn> Board::findPawnByName(const std::string& name, const size_t position) {
	std::vector query_result = pawns.findAllByName(name);
	if (query_result.size() > position) {
		return query_result[position];
	}

	FAULT("Trying to get nonexistent Pawn!"); //TODO
}


std::vector<std::shared_ptr<Pawn>> Board::findPawnsByName(const std::string& name) {
	return pawns.findAllByName(name);
}


void Board::printBoardTree() {
	out::debug("Entity Tree:\n%s", (pawns.toString() + " ").c_str());
}

void Board::printBoardTreeVerbose() {
	out::debug("Entity Tree:\n%s", (pawns.toStringVerbose() + " ").c_str());
}

void Board::setBoardName(const std::string& new_name) {
	name = new_name;
}


std::string Board::getBoardName() {
	return name;
}

void Board::setCameraPawn(const std::shared_ptr<SpatialPawn>& new_camera_pawn) {
	camera_pawn = new_camera_pawn;
}

glm::vec3 Board::getCamPos() const {
	return camera_pawn.lock()->getPosition();
}

glm::vec3 Board::getCamForward() const {
	return camera_pawn.lock()->getForwardVector();
}

Board::~Board() {
	delete pawns_to_remove;
	delete pawns_to_remove_from_hashmap;
}

void Board::dequeueRemove(const size_t amount) {
	while (!pawns_to_remove->empty()) {
		std::shared_ptr<Pawn> to_be_removed = pawns_to_remove->front();

		std::destroy(to_be_removed->children.begin(), to_be_removed->children.end());

		if (std::weak_ptr parent = to_be_removed->parent; !parent.expired()) {
			auto& children = parent.lock()->getChildren();
			std::erase_if(children,
			              [id = to_be_removed->id](const std::shared_ptr<Pawn>& x) {
				              return x->id == id;
			              });
		}

		to_be_removed->parent.reset();

#if ENGINE_DEBUG
		if (!to_be_removed.get()->is_tracked_on_hash) {
			//pawn should die here
			std::weak_ptr<Pawn> check = to_be_removed;
			pawns_to_remove->pop();

			if (!check.expired()) {
				FAULT("There is some reference to a pawn, not good...");
			}
		} else {
			pawns_to_remove->pop();
		}

#else
        pawns_to_remove->pop();
#endif
	}
	for (size_t i = 0;
	     i < (pawns_to_remove_from_hashmap->size() > amount ? amount : pawns_to_remove_from_hashmap->size()); i++) {
		const std::shared_ptr<Pawn> to_be_removed = pawns_to_remove_from_hashmap->front();

#if ENGINE_DEBUG
		if (!to_be_removed.get()->is_tracked_on_hash) {
			FAULT("Trying to remove pawn from hashmap that is explicitly mark for net being in the hashmap");
		}
#endif
		pawns.removeFromMaps(to_be_removed->name, to_be_removed->id);

		pawns_to_remove_from_hashmap->pop();
	}
	for (size_t i = 0; i < (components_to_remove->size() > amount ? amount : components_to_remove->size()); i++) {
		std::shared_ptr<Component>* to_be_removed = &components_to_remove->front();

		//todo removing from components hashmap after adding it

		components_to_remove->pop();
	}
}

int Board::pawnsToRemove() const {
	return pawns_to_remove->size();
}

void Board::registerPhysicsComponent(const std::shared_ptr<PhysicsComponent>& physics_component) {
	pawns.registerPhysicsComponent(physics_component);
}

void Board::removePhysicsComponent(const std::shared_ptr<PhysicsComponent>& physics_component) {
	pawns.removePhysicsComponent(physics_component);
}
