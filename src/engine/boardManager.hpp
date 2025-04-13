#pragma once
#include "external.hpp"
#include "render/window.hpp"
#include "shared/thread/phased.hpp"
#include "shared/thread/mailbox.hpp"

enum BoardRevocery {
	None,
	Default,
	Search
};

class Board;

class BoardManager {
protected:
	unsigned long long globalTickNumber;
	std::weak_ptr<Board> current_board;
	std::vector<std::shared_ptr<Board>> boardList;
	///board that generates when current_board suddenly disappears, (to avoid crashin the program), needs to be set by user
	std::weak_ptr<Board> default_board;
	///current board recovery mode
	BoardRevocery board_revocery;

	std::unique_ptr<PhasedTaskDelegator> task_delegator;
	TaskPool task_pool;
	std::thread physics_thread;

	std::atomic<bool> continue_loop;

	/*
	 * Creates standard setup of objects and components
	 */
	void standardSetup();

	void addBoard(const std::shared_ptr<Board>& new_board);

public:

	explicit BoardManager();

	~BoardManager();

	/**
	 * facilitates standard update
	 */
	void updateCycle();

	/**
	 * facilitates fixed update
	 */
	[[noreturn]] void fixedUpdateCycle();

	/**
	 * if board expires it tries to load other one if board recovery is set to true...
	 */
	std::shared_ptr<Board> findWorkingBoard(bool& success);

	/**
	 * returns currently used board
	 */
	std::weak_ptr<Board> getCurrentBoard();
};