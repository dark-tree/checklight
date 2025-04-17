#include "boardManager.hpp"
#include "board.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"
#include "engine/entity/component/camera.hpp"
#include "shared/logger.hpp"
#include <chrono>

/*
 * BoardManager
 */

BoardManager::BoardManager(Window &window) {
	std::shared_ptr<Board> new_board = std::make_shared<Board>();
	continue_loop = true;

	taskDelegator = std::make_unique<PhasedTaskDelegator>(taskPool);

	physicsDelegator = std::make_unique<MailboxTaskDelegator>(taskPool);

	globalTickNumber = 0;
	boardList.push_back(new_board);
	current_board = new_board;
	w = &window;
	standardSetup();

	/*
	physicsDelegator->enqueue( [this]() {
		fixedUpdateCycle();
	});*/
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
	/*
	if(globalTickNumber == 3000){
		usingBoard->pawns.findByID(4)->remove();
	}*/

	/*
	auto p = std::make_shared<Pawn>();
	usingBoard->addPawnToRoot(p);*/

	//communicate with renderer


	//remove
	usingBoard->dequeueRemove(100);

	//debug


	if(globalTickNumber % 2000 == 1){
		//out::info("tick: %d", globalTickNumber);
		//usingBoard->printBoardTreeVerbose();
	}
	globalTickNumber++;
}


void BoardManager::fixedUpdateCycle() {
	// double sum = 0.0;
	// int amount = 0;

	auto nextTick = std::chrono::steady_clock::now();
	// auto start = std::chrono::steady_clock::now();
	while(continue_loop){
		nextTick = nextTick + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
				std::chrono::duration<double>(TICK_DURATION));

		if (!current_board.expired()) {
			current_board.lock()->fixedUpdateBoard();
		}
		// auto end = std::chrono::steady_clock::now();
		// double tick_calculation_time = std::chrono::duration<double>(end - start).count();
		// sum += tick_calculation_time;
		// amount++;
		// start = end;
		//
		// printf("%f %f\n", sum/(double)amount, tick_calculation_time);
		std::this_thread::sleep_until(nextTick);
	}
}


std::weak_ptr<Board> BoardManager::getCurrentBoard() {
	return current_board;
}

BoardManager::~BoardManager() {
	continue_loop = false;
}
