#include "context.hpp"

/*
 * Context
 */


Context::Context(const float delta, const std::shared_ptr<Pawn>& pawn) {
	parent_pawn = pawn;
	deltaTime = delta;
}
