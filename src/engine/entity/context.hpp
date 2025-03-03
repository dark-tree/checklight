#pragma once
#include "pawn.hpp"

struct Context {
	double deltaTime;
	std::shared_ptr<Pawn> parentPawn;

	Context(float delta, std::shared_ptr<Pawn>& p);
};

struct FixedContext {
	std::shared_ptr<Pawn> parentPawn;
};