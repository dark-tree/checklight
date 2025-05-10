#pragma once

#include "render/immediate.hpp"
#include "layout/state.hpp"
#include "input/event.hpp"

class WidgetContext;

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
