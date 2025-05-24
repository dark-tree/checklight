#include "overlay.hpp"
#include "context.hpp"

bool Overlay::shouldUnbind() {
	return removal;
}

void Overlay::unbind() {
	removal = true;
}

void Overlay::bind(bool bound) {
	if (!bound) removal = false;
}

void Overlay::draw(ImmediateRenderer& immediate) {
	// do nothing
}

bool Overlay::event(WidgetContext& context, const InputEvent& event) {
	// do nothing
	return false;
}
