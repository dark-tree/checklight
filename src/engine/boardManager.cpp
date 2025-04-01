#include "boardManager.hpp"
#include "board.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"
#include "engine/entity/component/camera.hpp"
#include "shared/logger.hpp"



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
	std::shared_ptr<Board> usingBoard;

	if(!current_board.expired()) usingBoard = current_board.lock();
	else{
		FAULT("unhandled yet...");
		//TODO
	}

	//update
	usingBoard->updateBoard();

	//hardcoded updates
	if(globalTickNumber == 3000){
		usingBoard->pawns.findByID(4)->remove();
	}

	/*
	auto p = std::make_shared<Pawn>();
	usingBoard->addPawnToRoot(p);*/

	//communicate with renderer


	//remove
	usingBoard->dequeueRemove(100);

	//debug

	/*
	if(globalTickNumber % 2000 == 1){
		out::info("tick: %d", globalTickNumber);
		usingBoard->printBoardTreeVerbose();
	}*/
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
