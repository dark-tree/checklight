#include "board.hpp"
#include "entity/pawns/spatialPawn.hpp"
#include "shared/logger.hpp"

/*
 * Board
 */

void Board::updateBoard() {
	pawns.updateTree();
}


void Board::fixedUpdateBoard() {
	pawns.fixedUpdateTree();
}


void Board::addPawnToRoot(const std::shared_ptr<Pawn>& pawn) {
	pawns.addToRoot(pawn);
	pawn->setBoard(this);
}


std::shared_ptr<Pawn> Board::findPawnByID(uint32_t id) {
	return pawns.findByID(id);
}


std::shared_ptr<Pawn> Board::findPawnByID(uint32_t id, size_t position) {
	std::vector query_result = pawns.findAllByID(id);
	if (query_result.size() > position) {
		return query_result[position];
	}

	FAULT("Trying to get nonexistent Pawn!"); //TODO
}


std::shared_ptr<Pawn> Board::findPawnByID(int32_t id) {
	if (id < 0) {
		FAULT("ID can't be negative!");
	}

	return pawns.findByID((uint32_t)id);
}


std::shared_ptr<Pawn> Board::findPawnByID(int32_t id, size_t position) {
	if (id < 0) {
		FAULT("ID can't be negative!");
	}

	std::vector query_result = pawns.findAllByID(static_cast<uint32_t>(id));
	if (query_result.size() > position) {
		return query_result[position];
	}

	FAULT("Trying to get nonexistent Pawn!"); //TODO
}


std::vector<std::shared_ptr<Pawn>> Board::findPawnsByID(int32_t id) {
	return pawns.findAllByID(id);
}


std::shared_ptr<Pawn> Board::findPawnByName(const std::string& name) {
	return pawns.findByName(name);
}


std::shared_ptr<Pawn> Board::findPawnByName(const std::string& name, size_t position) {
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
	out::debug("Entity Tree:\n%s",(pawns.toString() + " ").c_str());
}

void Board::printBoardTreeVerbose() {
	out::debug("Entity Tree:\n%s",(pawns.toStringVerbose() + " ").c_str());
}

void Board::setBoardName(const std::string& new_name) {
	name = new_name;
}


std::string Board::getBoardName() {
	return name;
}

void Board::setCameraPawn(std::shared_ptr<SpatialPawn>& new_cameraPawn) {
    cameraPawn = new_cameraPawn;
}

glm::vec3 Board::getCamPos() {
    return cameraPawn.lock()->getPosition();
}

glm::vec3 Board::getCamForward() {
    return cameraPawn.lock()->getForwardVector();
}




