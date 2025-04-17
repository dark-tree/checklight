
#include "external.hpp"
#include <vstl.hpp>

// checklight include
#include <gui/gui.hpp>
#include <render/render.hpp>

#include "shared/args.hpp"
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

TEST(util_weighed_set_repeated) {

	WeighedSet<int, std::greater<>> set;

	set.insert(20, 111);
	set.insert(10, 132);
	set.insert(10, 321);
	set.insert(10, 213);

	CHECK(set.size(), 4);
	int sum = 0;

	for (auto& i : set) {
		sum += i;
	}

	CHECK(sum, 777);
	set.remove(321);

	CHECK(set.size(), 3);

};

TEST(util_program_args) {

	const char* argv[] = {
		"program.exe", "--verbose", "--test", "value", "--another", "-f", "X", "-r", "-w"
	};

	int argc = sizeof(argv) / sizeof(char*);

	Args args(argc, argv);

	CHECK(args.name(), "program.exe");

	ASSERT(args.has("--verbose"));
	ASSERT(args.has("--test"));
	ASSERT(args.has("--another"));
	ASSERT(args.has("-f"));
	ASSERT(args.has("-r"));
	ASSERT(args.has("-w"));

	CHECK(args.get("--test"), "value");
	CHECK(args.get("-f"), "X");

	CHECK(args.get("--verbose"), "");
	CHECK(args.get("--another"), "");
	CHECK(args.get("-r"), "");
	CHECK(args.get("-w"), "");

};

TEST(gui_simple_padded_absolute) {

	for (Flow flow : {Flow::LEFT_TO_RIGHT, Flow::RIGHT_TO_LEFT, Flow::TOP_TO_BOTTOM, Flow::BOTTOM_TO_TOP}) {

		auto context = std::make_shared<WidgetContext>();
		auto root = std::make_shared<RootWidget>(10, 10);
		auto sub = std::make_shared<PanelWidget>();

		root->addWidget(sub);
		context->setRoot(root);

		root->width = Unit::px(400);
		root->height = Unit::px(400);
		root->padding = Unit::px(10);
		root->flow = flow;

		sub->width = Unit::px(100);
		sub->height = Unit::px(100);

		// manually build layout as normally that would happen on first draw()
		root->rebuildLayout();

		CHECK(root->content.x, 10);
		CHECK(root->content.y, 10);
		CHECK(root->content.w, 400);
		CHECK(root->content.h, 400);

		CHECK(root->padded.x, 0);
		CHECK(root->padded.y, 0);
		CHECK(root->padded.w, 420);
		CHECK(root->padded.h, 420);

		CHECK(sub->content.x, 10);
		CHECK(sub->content.y, 10);
		CHECK(sub->content.w, 100);
		CHECK(sub->content.h, 100);

		CHECK(sub->padded.x, 10);
		CHECK(sub->padded.y, 10);
		CHECK(sub->padded.w, 100);
		CHECK(sub->padded.h, 100);

	}

};

TEST(gui_simple_padded_fit) {

	for (Flow flow : {Flow::LEFT_TO_RIGHT, Flow::RIGHT_TO_LEFT, Flow::TOP_TO_BOTTOM, Flow::BOTTOM_TO_TOP}) {

		auto context = std::make_shared<WidgetContext>();
		auto root = std::make_shared<RootWidget>(10, 10);
		auto sub = std::make_shared<PanelWidget>();

		root->addWidget(sub);
		context->setRoot(root);

		root->width = Unit::fit();
		root->height = Unit::fit();
		root->flow = flow;

		sub->width = Unit::px(100);
		sub->height = Unit::px(100);
		sub->padding = {Unit::px(20), Unit::px(10)};

		// manually build layout as normally that would happen on first draw()
		root->rebuildLayout();

		CHECK(root->content.x, 10);
		CHECK(root->content.y, 10);
		CHECK(root->content.w, 120);
		CHECK(root->content.h, 140);

		CHECK(sub->content.x, 20);
		CHECK(sub->content.y, 30);
		CHECK(sub->content.w, 100);
		CHECK(sub->content.h, 100);

	}

};

TEST(gui_simple_inverse) {

	auto context = std::make_shared<WidgetContext>();
	auto root = std::make_shared<RootWidget>();
	auto sub1 = std::make_shared<PanelWidget>();
	auto sub2 = std::make_shared<PanelWidget>();

	root->addWidget(sub1);
	root->addWidget(sub2);
	context->setRoot(root);

	root->width = Unit::fit();
	root->height = Unit::fit();
	root->flow = Flow::RIGHT_TO_LEFT;

	sub1->width = Unit::px(100);
	sub1->height = Unit::px(100);

	sub2->width = Unit::px(100);
	sub2->height = Unit::px(100);
	sub2->margin ={ Unit::px(100), Unit::px(200), Unit::px(10), Unit::zero()};

	// manually build layout as normally that would happen on first draw()
	root->rebuildLayout();

	CHECK(sub1->content.x, 110);
	CHECK(sub2->content.x, 10);

};

TEST(gui_layout_simple_grow) {

	auto context = std::make_shared<WidgetContext>();
	auto root = std::make_shared<RootWidget>();
	auto sub1 = std::make_shared<PanelWidget>();
	auto sub2 = std::make_shared<PanelWidget>();

	root->addWidget(sub1);
	root->addWidget(sub2);
	context->setRoot(root);

	root->width = Unit::px(400);
	root->height = Unit::fit();
	root->flow = Flow::LEFT_TO_RIGHT;

	sub1->width = Unit::grow(1);
	sub1->height = Unit::px(100);

	sub2->width = Unit::grow(3);
	sub2->height = Unit::grow(1);

	// manually build layout as normally that would happen on first draw()
	root->rebuildLayout();

	CHECK(root->content.x, 0);
	CHECK(root->content.y, 0);
	CHECK(root->content.w, 400);
	CHECK(root->content.h, 100);

	CHECK(sub1->content.x, 0);
	CHECK(sub1->content.y, 0);
	CHECK(sub1->content.w, 100);
	CHECK(sub1->content.h, 100);

	CHECK(sub2->content.x, 100);
	CHECK(sub2->content.y, 0);
	CHECK(sub2->content.w, 300);
	CHECK(sub2->content.h, 100);

};

TEST(gui_layout_complex_grow) {

	auto context = std::make_shared<WidgetContext>();
	auto root = std::make_shared<RootWidget>();
	auto sub1 = std::make_shared<PanelWidget>();
	auto sub2 = std::make_shared<PanelWidget>();
	auto sub3 = std::make_shared<PanelWidget>();

	root->addWidget(sub1);
	root->addWidget(sub2);
	sub1->addWidget(sub3);
	context->setRoot(root);

	root->width = Unit::px(100);
	root->height = Unit::fit();
	root->flow = Flow::LEFT_TO_RIGHT;

	sub1->width = Unit::fit();
	sub1->height = Unit::fit();
	sub1->padding = Unit::px(10);

	sub2->width = Unit::grow();
	sub2->height = Unit::grow();

	sub3->width = Unit::px(50);
	sub3->height = Unit::px(100);

	// manually build layout as normally that would happen on first draw()
	root->rebuildLayout();

	CHECK(sub2->content.x, 70);
	CHECK(sub2->content.y, 0);
	CHECK(sub2->content.w, 30);
	CHECK(sub2->content.h, 120);

};

TEST(gui_fit_text) {

	// print errors only
	out::logger.setLogLevelMask(Logger::Level::ERROR);

	ApplicationParameters parameters;
	parameters.setName("Test 'gui_fit_text'");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);

	const char* text =  "Many words that fit in thee";

	auto context = std::make_shared<WidgetContext>();
	auto root = std::make_shared<RootWidget>();
	auto sub1 = std::make_shared<TextWidget>(text);

	root->addWidget(sub1);
	context->setRoot(root);

	root->width = Unit::fit();
	root->height = Unit::fit();

	auto bakery = sub1->getBakery(0, 0, ElementState::ofLayout());
	bakery.setWrapping(false);
	auto metric = bakery.bakeString(0, 0, text).getMetrics();

	// manually build layout as normally that would happen on first draw()
	root->rebuildLayout();

	// check if the text did not wrap in fit container
	CHECK(sub1->content.h, metric.height);

	RenderSystem::system.reset();

};

TEST(gui_alignment_along_horizontal) {

	auto context = std::make_shared<WidgetContext>();
	auto root = std::make_shared<RootWidget>();
	auto sub1 = std::make_shared<PanelWidget>();
	auto sub2 = std::make_shared<PanelWidget>();

	root->addWidget(sub1);
	root->addWidget(sub2);
	context->setRoot(root);

	root->width = Unit::px(400);
	root->height = Unit::px(400);
	root->flow = Flow::LEFT_TO_RIGHT;

	sub1->width = Unit::px(100);
	sub1->height = Unit::px(100);

	sub2->width = Unit::px(100);
	sub2->height = Unit::px(100);

	root->horizontal = HorizontalAlignment::CENTER;

	// manually build layout as normally that would happen on first draw()
	root->rebuildLayout();

	CHECK(sub1->content.x, 100);
	CHECK(sub1->content.y, 0);

	CHECK(sub2->content.x, 200);
	CHECK(sub2->content.y, 0);

	root->horizontal = HorizontalAlignment::RIGHT;

	// force layout update
	root->update();
	root->rebuildLayout();

	CHECK(sub1->content.x, 200);
	CHECK(sub1->content.y, 0);

	CHECK(sub2->content.x, 300);
	CHECK(sub2->content.y, 0);

};

TEST(gui_rebuild_propagation) {

	auto context = std::make_shared<WidgetContext>();
	auto root = std::make_shared<RootWidget>();
	auto sub1 = std::make_shared<PanelWidget>();
	auto sub2 = std::make_shared<PanelWidget>();

	auto nested = std::make_shared<PanelWidget>();
	nested->width = Unit::grow();
	nested->height = Unit::grow();

	root->addWidget(sub1);
	root->addWidget(sub2);
	context->setRoot(root);

	root->width = Unit::px(400);
	root->height = Unit::fit();
	root->flow = Flow::LEFT_TO_RIGHT;

	sub1->width = Unit::grow(1);
	sub1->height = Unit::px(100);

	sub2->width = Unit::grow(3);
	sub2->height = Unit::grow(1);

	sub2->addWidget(nested);

	// manually build layout as normally that would happen on first draw()
	root->rebuildLayout();

	CHECK(sub1->content.x, 0);
	CHECK(sub1->content.y, 0);
	CHECK(sub1->content.w, 100);
	CHECK(sub1->content.h, 100);

	CHECK(sub2->content.x, 100);
	CHECK(sub2->content.y, 0);
	CHECK(sub2->content.w, 300);
	CHECK(sub2->content.h, 100);

	sub1->width = Unit::grow(3);
	sub2->width = Unit::grow(1);

	// call on any child in tree, the request should propagate to root
	nested->update();

	root->rebuildLayout();

	CHECK(sub1->content.x, 0);
	CHECK(sub1->content.y, 0);
	CHECK(sub1->content.w, 300);
	CHECK(sub1->content.h, 100);

	CHECK(sub2->content.x, 300);
	CHECK(sub2->content.y, 0);
	CHECK(sub2->content.w, 100);
	CHECK(sub2->content.h, 100);

};

TEST(gui_minimal_fit) {

	auto context = std::make_shared<WidgetContext>();
	auto root = std::make_shared<RootWidget>();

	auto vertical = std::make_shared<PanelWidget>();
	vertical->flow = Flow::TOP_TO_BOTTOM;

	auto item_1 = std::make_shared<PanelWidget>();
	item_1->min_height = Unit::px(10);
	item_1->min_width = Unit::px(10);
	vertical->addWidget(item_1);

	auto item_2 = std::make_shared<PanelWidget>();
	item_2->min_height = Unit::px(10);
	item_2->min_width = Unit::px(10);
	vertical->addWidget(item_2);

	auto item_3 = std::make_shared<PanelWidget>();
	item_3->min_height = Unit::px(10);
	item_3->min_width = Unit::px(10);
	vertical->addWidget(item_3);

	root->addWidget(vertical);
	root->rebuildLayout();

	CHECK(root->content.x, 0);
	CHECK(root->content.y, 0);
	CHECK(root->content.w, 10);
	CHECK(root->content.h, 30);

};

TEST(style_interpolate_numericals) {

	int i0 = interpolate(2, 10, 0);
	int i1 = interpolate(2, 10, 0.5);
	int i2 = interpolate(2, 10, 1);

	float f0 = interpolate(2.0f, 10.0f, 0);
	float f1 = interpolate(2.0f, 10.0f, 0.5);
	float f2 = interpolate(2.0f, 10.0f, 1);

	CHECK(i0, 2);
	CHECK(i1, 6);
	CHECK(i2, 10);

	CHECK(f0, 2.0f);
	CHECK(f1, 6.0f);
	CHECK(f2, 10.0f);

};

TEST(style_resolve_units) {

	Viewport::setCurrent(100, 2000);

	int w = Unit::vw(0.1).pixels();
	int h = Unit::vh(0.1).pixels();

	CHECK(w, 10);
	CHECK(h, 200);

}

TEST(style_interpolate_units) {

	Viewport::setCurrent(100, 100);

	Unit u0 = interpolate(Unit::px(2), Unit::px(10), 0.5f);
	Unit u1 = interpolate(Unit::px(3), Unit::fit(), 0.6f);
	Unit u2 = interpolate(Unit::px(4), Unit::fit(), 0.3f);
	Unit u3 = interpolate(Unit::vh(0), Unit::vh(100), 0.3f);
	Unit u4 = interpolate(Unit::vw(0.1), Unit::vh(0.5), 0.5f);

	CHECK(u0.metric, 2);
	CHECK(u0.value, 6);

	CHECK(u1.metric, Metric::FIT);
	CHECK(u2.metric, Metric::PIXELS);
	CHECK(u2.value, 4);

	CHECK(u3.metric, Metric::VIEWPORT_HEIGHT);
	CHECK(static_cast<int>(u3.value), 30);

	CHECK(u4.metric, Metric::PIXELS);
	CHECK(static_cast<int>(u4.value), 30);

};