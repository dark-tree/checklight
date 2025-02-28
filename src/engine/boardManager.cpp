#include "boardManager.hpp"
#include "board.hpp"

/*
 * BoardManager
 */

BoardManager::BoardManager() {
	std::shared_ptr<Board> new_board = std::make_shared<Board>();

	boardList.push_back(new_board);
	current_board = new_board;
}


void BoardManager::updateCycle() {

	//initate


	//update
	if (!current_board.expired()) {
		current_board.lock()->updateBoard();
	}

	//communicate with renderer


	//remove


	//debug
	//temporary
	if (globalTickNumber % 1000 == 0) {
		if (!current_board.expired()) {
			std::string result = current_board.lock()->printBoardTree();
			printf(result.c_str());
		}
	}

	globalTickNumber++;
}


void BoardManager::fixedUpdateCycle() {
	if (!current_board.expired()) {
		current_board.lock()->fixedUpdateBoard();
	}
}


std::weak_ptr<Board> BoardManager::getCurrentBoard() {
	return current_board;
}