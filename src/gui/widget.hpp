#pragma once

#include "external.hpp"
#include <input/listener.hpp>
#include <input/event/mouse.hpp>

class ImmediateRenderer;

class Widget : public InputListener {

	protected:

		/// position and dimensions, can be changed using setBounds()
		int x, y, w, h;

		bool enabled = true;
		bool hovered = false;
		bool pressed = false;

	public:

		virtual ~Widget();
		virtual void draw(ImmediateRenderer& immediate) = 0;
		virtual bool handle(const InputEvent& event);

		InputResult onEvent(const InputEvent& event) final;

	public:

		void setBounds(int x, int y, int w, int h);

};