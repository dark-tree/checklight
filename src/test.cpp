
#include "external.hpp"
#include <vstl.hpp>

// checklight include
#include <engine/board.hpp>
#include <engine/boardManager.hpp>
#include <gui/gui.hpp>
#include <render/render.hpp>

#include "shared/args.hpp"
#include "shared/pyramid.hpp"
#include "shared/weighed.hpp"

BEGIN(VSTL_MODE_LENIENT)



//-------------------------------------TESTING ENGINE----------------------------------------

#define BOARD_SETUP BoardManager manager; std::shared_ptr<Board> board = manager.getCurrentBoard().lock();

TEST(pawn_tree_pawn_addition) {
	BOARD_SETUP

	//check simple addition
	std::shared_ptr<Pawn> r1 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r2 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r3 = std::make_shared<Pawn>();

	board->addPawnToRoot(r1);
	board->addPawnToRoot(r2);
	board->addPawnToRoot(r3);

	CHECK(board.get()->getTree().getRoot()->getChildren()[1],r1);
	CHECK(board.get()->getTree().getRoot()->getChildren()[2],r2);
	CHECK(board.get()->getTree().getRoot()->getChildren()[3],r3);

	//check different order of addition
	std::shared_ptr<Pawn> r4 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r5 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r6 = std::make_shared<Pawn>();

	r4->addChild(r5);
	board->addPawnToRoot(r4);

	r5->addChild(r6);

	CHECK(board.get()->getTree().getRoot()->getChildren()[4],r4);
	CHECK(board.get()->getTree().getRoot()->getChildren()[4]->getChildren()[0],r5);
	CHECK(board.get()->getTree().getRoot()->getChildren()[4]->getChildren()[0]->getChildren()[0],r6);

	//check extra branching out on a deep branch
	std::shared_ptr<Pawn> r7 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r8 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r9 = std::make_shared<Pawn>();

	r6->addChild(r7);
	r6->addChild(r8);
	r6->addChild(r9);

	CHECK(board.get()->getTree().getRoot()->getChildren()[4]->getChildren()[0]->getChildren()[0]->getChildren()[0],r7);
	CHECK(board.get()->getTree().getRoot()->getChildren()[4]->getChildren()[0]->getChildren()[0]->getChildren()[1],r8);
	CHECK(board.get()->getTree().getRoot()->getChildren()[4]->getChildren()[0]->getChildren()[0]->getChildren()[2],r9);
};

TEST(search_by_name) {
	BOARD_SETUP

	std::shared_ptr<Pawn> r1 = std::make_shared<Pawn>("r1");
	std::shared_ptr<Pawn> r2 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r3 = std::make_shared<Pawn>("r3");

	CHECK(board->findPawnByName("r1"),nullptr);

	board->addPawnToRoot(r1);
	board->addPawnToRoot(r2);
	board->addPawnToRoot(r3);

	CHECK(board->findPawnByName("r1"),r1);
	CHECK(board->findPawnByName("r2"),nullptr);
	CHECK(board->findPawnByName("r3"),r3);

	std::shared_ptr<Pawn> r4 = std::make_shared<Pawn>("r4");
	std::shared_ptr<Pawn> r5 = std::make_shared<Pawn>("r5");
	std::shared_ptr<Pawn> r6 = std::make_shared<Pawn>("r6");

	r4->addChild(r5);
	board->addPawnToRoot(r4);
	r5->addChild(r6);

	CHECK(board->findPawnByName("r6"),r6);
}

TEST(search_by_id) {
	BOARD_SETUP

	std::shared_ptr<Pawn> r1 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r2 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r3 = std::make_shared<Pawn>();
	int id1 = r1->getEntityID();
	int id2 = r2->getEntityID();
	int id3 = r3->getEntityID();

	CHECK(board->findPawnByID(id1),nullptr);

	board->addPawnToRoot(r1);
	board->addPawnToRoot(r2);
	board->addPawnToRoot(r3);

	CHECK(board->findPawnByID(id1),r1);
	CHECK(board->findPawnByID(id2<<2),nullptr);
	CHECK(board->findPawnByID(id3),r3);

	std::shared_ptr<Pawn> r4 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r5 = std::make_shared<Pawn>();
	std::shared_ptr<Pawn> r6 = std::make_shared<Pawn>();

	int id6 = r6->getEntityID();

	r4->addChild(r5);
	board->addPawnToRoot(r4);
	r5->addChild(r6);

	CHECK(board->findPawnByID(id6),r6);
}

TEST(get_multiple_by_name) {
	BOARD_SETUP

	std::shared_ptr<Pawn> a[10];
	std::shared_ptr<Pawn> b[7];

	for (int i = 0; i < 10; i++) {
		a[i] = std::make_shared<Pawn>("SetA");
	}

	for (int i = 0; i < 7; i++) {
		b[i] = std::make_shared<Pawn>("SetB");
	}

	CHECK(board->findPawnByName("SetA"), nullptr);
	ASSERT(board->findPawnsByName("SetA").empty());

	for (int i = 0; i < 10; i++) {
		board->addPawnToRoot(a[i]);
	}

	for (int i = 0; i < 7; i++) {
		board->addPawnToRoot(b[i]);
	}

	CHECK(board->findPawnsByName("SetA").size(), 10);
	CHECK(board->findPawnsByName("SetB").size(), 7);
	ASSERT(board->findPawnsByName("SetC").empty());

	CHECK(board->findPawnByName("SetA")->getName(), "SetA");
	CHECK(board->findPawnByName("SetB")->getName(), "SetB");
}

TEST(get_multiple_by_id) {
	BOARD_SETUP

	//cant check for multiple instances of the same id (would require creating more than bilion elements)
	std::shared_ptr<Pawn> a[10];

	for (int i = 0; i < 10; i++) {
		a[i] = std::make_shared<Pawn>("SetA");
		board->addPawnToRoot(a[i]);
	}

	for (int i = 0; i < 10; i++) {
		CHECK(board->findPawnsByID(a[i]->getEntityID()).size(),1);
		CHECK(board->findPawnsByID(a[i]->getEntityID())[0],a[i]);
	}
}

TEST(remove_pawn) {
	BOARD_SETUP

	std::shared_ptr<Pawn> r1 = std::make_shared<Pawn>("r1");
	std::shared_ptr<Pawn> r2 = std::make_shared<Pawn>("r2");
	std::shared_ptr<Pawn> r3 = std::make_shared<Pawn>("r3");

	board->addPawnToRoot(r1);
	board->addPawnToRoot(r2);
	board->addPawnToRoot(r3);

	r2->remove();

	CHECK(board.get()->getTree().getRoot()->getChildren().size(),4);

	CHECK(board->findPawnByName("r1"),r1);
	CHECK(board->findPawnByName("r2"),r2);
	CHECK(board->findPawnByName("r3"),r3);

	manager.updateCycle();

	CHECK(board.get()->getTree().getRoot()->getChildren().size(),3);
	CHECK(board.get()->getTree().getRoot()->getChildren()[1],r1);
	CHECK(board.get()->getTree().getRoot()->getChildren()[2],r3);

	CHECK(board->findPawnByName("r1"),r1);
	CHECK(board->findPawnByName("r2"),nullptr);
	CHECK(board->findPawnByName("r3"),r3);

	std::weak_ptr<Pawn> w1 = std::weak_ptr<Pawn>(r1);
	std::weak_ptr<Pawn> w2 = std::weak_ptr<Pawn>(r2);
	std::weak_ptr<Pawn> w3 = std::weak_ptr<Pawn>(r3);

	r1.reset();
	r2.reset();
	r3.reset();

	ASSERT(!w1.expired());
	ASSERT(w2.expired());
	ASSERT(!w3.expired());
};

TEST() {
	BOARD_SETUP
};


