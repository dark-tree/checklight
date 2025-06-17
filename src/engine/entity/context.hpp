#pragma once
#include "pawn.hpp"

struct Context {
	double delta;
	std::shared_ptr<Pawn> parent_pawn;

	Context(float delta, const std::shared_ptr<Pawn>& pawn);
};

struct FixedContext {
	std::shared_ptr<Pawn> parent_pawn;
};
