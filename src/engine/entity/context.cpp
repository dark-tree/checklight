#include "context.hpp"

/*
 * Context
 */


Context::Context(const float delta, const std::shared_ptr<Pawn>& parent_pawn) {
	this->parent_pawn = parent_pawn;
	this->delta = delta;
}
