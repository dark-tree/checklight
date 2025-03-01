#pragma once
#include "external.hpp"

class Board;

class BoardManager {
protected:
	unsigned long long globalTickNumber;
	std::weak_ptr<Board> current_board;
	std::vector<std::shared_ptr<Board>> boardList;

public:

	BoardManager();

	/*
	 * Creates standard setup of objects and components 
	 */
	void standardSetup();

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
};