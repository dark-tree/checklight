#pragma once
#include "pawnTree.hpp"

class SpatialPawn;

//Its a Scene dumbass
class Board : public std::enable_shared_from_this<Board> {
private:
	friend class BoardManager;
	friend class Pawn;
	PawnTree pawns;
	std::weak_ptr<SpatialPawn> cameraPawn;
	std::string name;
	std::queue<std::shared_ptr<Pawn>>* pawns_to_remove;
	std::queue<std::shared_ptr<Pawn>>* pawns_to_remove_from_hashmap;

	/**
	 * queue remove a pawn
	 */
	 void queueRemove(const std::shared_ptr<Pawn>& pToRemove);

	 void dequeueRemove(size_t amount);

public:
	Board();

	~Board();

	/**
	 * performs standard update on a pawn tree
	 */
	void updateBoard();

	/**
	 * performs fixed update on a pawn tree
	 */
	void fixedUpdateBoard();

	/**
	 * adds a pawn as a child of pawn tree
	 */
	void addPawnToRoot(const std::shared_ptr<Pawn>& pawn);

	/**
	 * returns first pawn by its id
	 */
	std::shared_ptr<Pawn> findPawnByID(uint32_t id);

	/**
	 * returns pawn by its id at certain position (if there are multiple pawns with the same Id, there shouldnt be)
	 */
	std::shared_ptr<Pawn> findPawnByID(uint32_t id, size_t position);

	/**
	 * returns first pawn by its id
	 */
	std::shared_ptr<Pawn> findPawnByID(int32_t id);

	/**
	 * returns pawn by its id at certain position (if there are multiple pawns with the same Id, there shouldnt be)
	 */
	std::shared_ptr<Pawn> findPawnByID(int32_t id, size_t position);

	/**
	 * returns list of all pawns with the same id (there should be one really)
	 */
	std::vector<std::shared_ptr<Pawn>> findPawnsByID(int32_t id);

	/**
	 * returns first pawn by its name
	 */
	std::shared_ptr<Pawn> findPawnByName(const std::string& name);

	/**
	 * returns first pawn by its name at certain position
	 */
	std::shared_ptr<Pawn> findPawnByName(const std::string& name, size_t position);

	/**
	 * returns all pawns with certain name
	 */
	std::vector<std::shared_ptr<Pawn>> findPawnsByName(const std::string& name);

	/**
	 * DEBUG ONLY - prints whole pawn tree (DONT USE IN RELEASE VERSION) 
	 */
	void printBoardTree();

	/**
	 * DEBUG ONLY - prints whole pawn tree with great detail (DONT USE IN RELEASE VERSION)
	 */
	void printBoardTreeVerbose();

	/**
	 * set name of a board
	 */
	void setBoardName(const std::string& new_name);

	/**
	 * return name of a board
	 */
	std::string getBoardName();

	void setCameraPawn(std::shared_ptr<SpatialPawn>& new_cameraPawn);

	glm::vec3 getCamPos();

	glm::vec3 getCamForward();
};