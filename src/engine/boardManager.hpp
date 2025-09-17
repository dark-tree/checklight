#pragma once

#include <physics/physicsEngine.hpp>

#include "external.hpp"
#include "render/window.hpp"
#include "shared/thread/phased.hpp"
#include "shared/thread/mailbox.hpp"


enum BoardRevocery {
	NONE,
	DEFAULT,
	SEARCH
};

class Board;

class BoardManager {
protected:
	PhysicsEngine physics_engine;
	unsigned long long global_tick_number;
	std::weak_ptr<Board> current_board;
	std::vector<std::shared_ptr<Board>> board_list;
	///board that generates when current_board suddenly disappears, (to avoid crashing the program), needs to be set by user
	std::weak_ptr<Board> default_board;
	///current board recovery mode
	BoardRevocery board_recovery;

	std::shared_ptr<InputDispatcher> dispatcher;

	std::unique_ptr<PhasedTaskDelegator> task_delegator;
	TaskPool task_pool;
	std::thread physics_thread;

	std::mutex physics_mutex;

	std::atomic<bool> continue_loop;
	std::chrono::time_point<std::chrono::steady_clock> physics_next_tick;

	/*
	 * Creates standard setup of objects and components
	 */
	void standardSetup();

	void addBoard(const std::shared_ptr<Board>& new_board);

	/**
	 * if board expires it tries to load other one if board recovery is set to true...
	 */
	std::shared_ptr<Board> findWorkingBoard(bool& success);

public:
	BoardManager(const std::shared_ptr<InputDispatcher>& disp = nullptr);

	~BoardManager();

	/**
	 * facilitates standard update
	 */
	void updateCycle();

	/**
	 * facilitates fixed update
	 */
	void fixedUpdateCycle();


	/**
	 * returns currently used board
	 */
	std::weak_ptr<Board> getCurrentBoard();

	/**
	 * sets gravity vector
	 */
	void setGravity(glm::vec3 gravity);
};
