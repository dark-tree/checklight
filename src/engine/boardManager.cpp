#include "boardManager.hpp"
#include "board.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"
#include "engine/entity/component/camera.hpp"
#include "shared/logger.hpp"
#include <chrono>
#include "physics/physicsEngine.hpp"

/*
 * BoardManager
 */

BoardManager::BoardManager(const std::shared_ptr<InputDispatcher>& disp): physics_engine({0.0, 0.0, 0.0}, this) {
	dispatcher = disp;

	const auto new_board = std::make_shared<Board>();
	addBoard(new_board);

	continue_loop = true;
	task_delegator = std::make_unique<PhasedTaskDelegator>(task_pool);
	global_tick_number = 0;

	standardSetup();
}

BoardManager::~BoardManager() {
	continue_loop = false;
	// physics_thread.join();
}

void BoardManager::standardSetup() {
	const auto cameraPawn = std::make_shared<SpatialPawn>();
	const auto cam = cameraPawn->createComponent<Camera>();

	if (dispatcher) dispatcher->registerListener(cam);

	cameraPawn->setName("Main Camera");

	std::shared_ptr<Board> cb = current_board.lock();

	std::shared_ptr<SpatialPawn> cameraPawn2 = std::static_pointer_cast<SpatialPawn>(cameraPawn);
	cb->setCameraPawn(cameraPawn2);
	cb->addPawnToRoot(cameraPawn);
}

void BoardManager::addBoard(const std::shared_ptr<Board>& new_board) {
	boardList.push_back(new_board);
	current_board = new_board;
}

void BoardManager::updateCycle() {
	//-----------static------------

	static auto before = std::chrono::high_resolution_clock::now();

	//-----------initate-----------

	std::shared_ptr<Board> usingBoard;

	if (!current_board.expired()) usingBoard = current_board.lock();
	else {
		bool success;
		current_board = findWorkingBoard(success);
		if (success) usingBoard = current_board.lock();
		else FAULT("There is no available board!");
	}

	//-----------update-------------

	const auto now = std::chrono::high_resolution_clock::now();
	usingBoard->updateBoard(std::chrono::duration<double>(now - before).count(), physics_mutex);
	before = now;


	auto next_tick = std::chrono::steady_clock::now();

	if (next_tick > physics_next_tick) {
		physics_next_tick = next_tick + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
			                    std::chrono::duration<double>(TICK_DURATION));

		if (!current_board.expired()) {
			current_board.lock()->fixedUpdateBoard();
			physics_engine.physicsUpdate();
		}
	}

	//------hardcoded updates-------

	/*
	auto p = std::make_shared<Pawn>();
	usingBoard->addPawnToRoot(p);*/

	//---communicate with renderer---


	//-----------remove--------------

	if (usingBoard->pawnsToRemove() > 0) {
		//maybe someday it will be smarter
		physics_mutex.lock();
		usingBoard->dequeueRemove(100);
		physics_mutex.unlock();
	}


	//-----------debug---------------

	//
	// if(globalTickNumber % 2000 == 1){
	// 	out::info("tick: %d", globalTickNumber);
	// 	usingBoard->printBoardTreeVerbose();
	// }
	global_tick_number++;
}


void BoardManager::fixedUpdateCycle() {
	// double sum = 0.0;
	// int amount = 0;

	// auto nextTick = std::chrono::steady_clock::now();
	// // auto start = std::chrono::steady_clock::now();
	// while(continue_loop){
	// 	nextTick = nextTick + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
	// 			std::chrono::duration<double>(TICK_DURATION));
	//
	// 	if (!current_board.expired()) {
	// 		physics_mutex.lock();
	//
	// 		current_board.lock()->fixedUpdateBoard();
	//
	// 		physics_engine.physicsUpdate();
	//
	// 		physics_mutex.unlock();
	// 	}
	// 	// auto end = std::chrono::steady_clock::now();
	// 	// double tick_calculation_time = std::chrono::duration<double>(end - start).count();
	// 	// sum += tick_calculation_time;
	// 	// amount++;
	// 	// start = end;
	// 	//
	// 	// printf("%f %f\n", sum/(double)amount, tick_calculation_time);
	// 	std::this_thread::sleep_until(nextTick);
	// }
	// out::info("%s", "Closing the physics thread...");
}

std::shared_ptr<Board> BoardManager::findWorkingBoard(bool& success) {
	std::shared_ptr<Board> new_board;
	switch (board_recovery) {
		case BoardRevocery::DEFAULT:
			if (!default_board.expired()) {
				new_board = default_board.lock();
				success = true;
			} else {
				success = false;
			}
			break;
		case BoardRevocery::NONE:
			success = false;
			break;
		case BoardRevocery::SEARCH:
			if (default_board.expired()) {
				new_board = default_board.lock();
				success = true;
			} else {
				if (boardList.size() > 0) {
					new_board = boardList[0];
					success = true;
				} else {
					success = false;
				}
			}
			break;
		default:
			success = false;
	}
	return new_board;
}


std::weak_ptr<Board> BoardManager::getCurrentBoard() {
	return current_board;
}

void BoardManager::setGravity(glm::vec3 gravity) {
	physics_engine.setGravityScale(gravity);
}
