#include "boardManager.hpp"
#include "board.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"
#include "engine/entity/component/camera.hpp"



/*
 * BoardManager
 */

BoardManager::BoardManager(Window &window) {
	std::shared_ptr<Board> new_board = std::make_shared<Board>();

	globalTickNumber = 0;
	boardList.push_back(new_board);
	current_board = new_board;
	w = &window;
	standardSetup();
}

void BoardManager::standardSetup(){
	std::shared_ptr<Pawn> cameraPawn = std::make_shared<SpatialPawn>();
	std::shared_ptr<Component> cam = std::make_shared<Camera>();
	w->getInputDispatcher().registerListener(cam); //TODO temporary

	cameraPawn->addComponent(cam);
	cameraPawn->setName("Main Camera");

	std::shared_ptr<Board> cb = current_board.lock();

	std::shared_ptr<SpatialPawn> cameraPawn2 = std::static_pointer_cast<SpatialPawn>(cameraPawn);
	cb->setCameraPawn(cameraPawn2);
	cb->addPawnToRoot(cameraPawn);
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
