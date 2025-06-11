#pragma once
#include "render/render.hpp"
#include "external.hpp"

class Models {
	inline static bool initialized = false;
	inline static std::vector<std::shared_ptr<RenderModel>> models;

public:
	enum Shape {
		SPHERE = 0,
		CUBE = 1
	};

	static void init();

	static void terminate();

	static std::shared_ptr<RenderModel> getShape(Shape s);
};
