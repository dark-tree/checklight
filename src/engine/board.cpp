#include "board.hpp"
#include "entity/pawns/spatialPawn.hpp"

/*
 * Board
 */

void Board::updateBoard() {
	pawns.updateTree();
}


void Board::fixedUpdateBoard() {
	pawns.fixedUpdateTree();
}


void Board::addPawnToRoot(std::shared_ptr<Pawn>& pawn) {
	pawns.addToRoot(pawn);
}


std::shared_ptr<Pawn> Board::findPawnByID(uint32_t id) {
	return std::move(pawns.findByID(id));
}


std::shared_ptr<Pawn> Board::findPawnByID(uint32_t id, size_t position) {
	std::vector query_result = std::move(pawns.findAllByID(id));
	if (query_result.size() > position) {
		return query_result[position];
	}
	else {
		throw std::runtime_error("trying to get nonexistent Pawn");
	}
}


std::shared_ptr<Pawn> Board::findPawnByID(int32_t id) {
	if (id < 0) {
		throw std::runtime_error("Id can't be negative");
	}
	else {
		return pawns.findByID((uint32_t)id);
	}
}


std::shared_ptr<Pawn> Board::findPawnByID(int32_t id, size_t position) {
	if (id < 0) {
		throw std::runtime_error("Id can't be negative");
	}
	else {
		std::vector query_result = std::move(pawns.findAllByID(static_cast<uint32_t>(id)));
		if (query_result.size() > position) {
			return query_result[position];
		}
		else {
			throw std::runtime_error("trying to get nonexistent Pawn");
		}
	}
}


std::vector<std::shared_ptr<Pawn>> Board::findPawnsByID(int32_t id) {
	return pawns.findAllByID(id);
}


std::shared_ptr<Pawn> Board::findPawnByName(const std::string& name) {
	return pawns.findByName(name);
}


std::shared_ptr<Pawn> Board::findPawnByName(const std::string& name, size_t position) {
	std::vector query_result = std::move(pawns.findAllByName(name));
	if (query_result.size() > position) {
		return query_result[position];
	}
	else {
		throw std::runtime_error("trying to get nonexistent Pawn");
	}
}


std::vector<std::shared_ptr<Pawn>> Board::findPawnsByName(const std::string& name) {
	return pawns.findAllByName(name);
}


std::string Board::printBoardTree() {
	return pawns.print();
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


