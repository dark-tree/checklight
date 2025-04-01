#pragma once
#include "external.hpp"
#include "render/window.hpp"
#include "shared/thread/phased.hpp"
#include "shared/thread/mailbox.hpp"

class Board;

class BoardManager {
protected:
	unsigned long long globalTickNumber;
	std::weak_ptr<Board> current_board;
	std::vector<std::shared_ptr<Board>> boardList;

	std::unique_ptr<PhasedTaskDelegator> taskDelegator;
	TaskPool taskPool;

	std::unique_ptr<MailboxTaskDelegator> physicsDelegator;
	std::atomic<bool> continueLoop;

	Window* w;
	/*
	 * Creates standard setup of objects and components
	 */
	void standardSetup();



public:

	explicit BoardManager(Window &window);

	/**
	 * facilitates standard update
	 */
	void updateCycle();

	/**
	 * facilitates fixed update
	 */
	[[noreturn]] void fixedUpdateCycle();

	/**
	 * returns currently used board
	 */
	std::weak_ptr<Board> getCurrentBoard();
};