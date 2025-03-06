#pragma once

#include "external.hpp"
#include <input/listener.hpp>
#include <input/event/mouse.hpp>

class ImmediateRenderer;

class Widget : public InputListener {

	protected:

		/// position and dimensions, can be changed using setBounds()
		int x, y, w, h;

	public:

		virtual ~Widget();
		virtual void draw(ImmediateRenderer& immediate) = 0;
		virtual bool handle(const InputEvent& event) = 0;

		InputResult onEvent(const InputEvent& event) final;

	public:

		void setBounds(int x, int y, int w, int h);

};