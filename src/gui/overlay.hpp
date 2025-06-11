#pragma once

#include "render/immediate.hpp"
#include "layout/state.hpp"
#include "input/event.hpp"

class WidgetContext;

/**
 * GUI overlays, renderable components that are drown over the rest of the GUI
 * and receive events before the rest of the GUI
 */
class Overlay {

	private:

		bool removal = false;

	public:

		virtual ~Overlay() = default;

		bool shouldUnbind();
		void unbind();

		virtual void bind(bool bound);
		virtual void draw(ImmediateRenderer& immediate);
		virtual bool event(WidgetContext& context, const InputEvent& event);

};
