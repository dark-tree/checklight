#include "context.hpp"

/*
 * Context
 */


Context::Context(float delta, std::shared_ptr<Pawn>& p) {
	parentPawn = p;
	deltaTime = 0.01f;
}