
#include "external.hpp"
#include <vstl.hpp>

// checklight include
#include "shared/pyramid.hpp"
#include "shared/weighed.hpp"

BEGIN(VSTL_MODE_LENIENT)

TEST(util_pyramid) {

	Pyramid<int> pyramid;
	ASSERT(pyramid.empty());

	pyramid.push();
	pyramid.append(4);
	pyramid.append(2);
	pyramid.append(1);
	CHECK(pyramid.size(), 1);

	pyramid.push();
	pyramid.append(2);
	pyramid.append(3);
	CHECK(pyramid.size(), 2);
	CHECK(pyramid.top().size(), 2);

	pyramid.push();
	pyramid.append(42);

	Pyramid<int>::View view = pyramid.view();

	std::set<int> level_1 {1, 2, 3, 4, 42};
	std::set<int> level_2 {2, 3, 42};
	std::set<int> level_3 {42};

	CHECK(view.collect(), level_1);
	view.up();
	CHECK(view.collect(), level_2);
	view.up();
	CHECK(view.collect(), level_3);
	view.up();
	ASSERT(view.collect().empty());
	view.up();
	ASSERT(view.collect().empty());

};

TEST(util_weighed_set) {

	WeighedSet<int, std::greater<>> set;

	ASSERT(set.empty());

	set.insert(11, 100);
	set.insert(10, 201);
	set.insert(100, 111);
	set.insert(99, 100);
	set.insert(1, 1000);
	set.insert(50, 200);

	CHECK(set.size(), 6);
	set.remove(1000);

	CHECK(set.size(), 5);

	std::vector<int> order;

	for (int i : set) {
		order.push_back(i);
	}

	CHECK(order[0], 111);
	CHECK(order[1], 100);
	CHECK(order[2], 200);
	CHECK(order[3], 100);
	CHECK(order[4], 201);

};

TEST(util_weighed_set_limit) {

	WeighedSet<int, std::greater<>> set;

	EXPECT(std::length_error, {
		set.lowest();
	});

	EXPECT(std::length_error, {
		set.highest();
	});

	set.insert(10, 101);

	CHECK(set.lowest(), set.highest());

	set.insert(20, 200);

	CHECK(set.lowest(), 10);
	CHECK(set.highest(), 20);

	set.insert(18, 400);

	CHECK(set.lowest(), 10);
	CHECK(set.highest(), 20);

};