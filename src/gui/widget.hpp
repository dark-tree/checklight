#pragma once

#include "external.hpp"
#include "input/input.hpp"

class WidgetContext;
class ImmediateRenderer;

class InputWidget;

class Widget : public std::enable_shared_from_this<Widget> {

	protected:

		/// position and dimensions, can be changed using setBounds()
		int x, y, w, h;

	public:

		virtual ~Widget();
		virtual void draw(ImmediateRenderer& immediate) = 0;
		virtual bool event(WidgetContext& context, const InputEvent& event);
		virtual void appendSelectable(std::vector<std::shared_ptr<InputWidget>>& selectable);

	public:

		void setBounds(int x, int y, int w, int h);

};

class InputWidget : public Widget {

	protected:

		bool enabled = true;
		bool hovered = false;
		bool pressed = false;
		bool selected = false;

		bool isFocused() const;
		void setFocus(WidgetContext& context);

	private:

		friend class WidgetContext;

		/// Never invoke this method yourself!
		void setSelected(bool selected);

	public:

		virtual bool event(WidgetContext& context, const InputEvent& event);
		virtual void appendSelectable(std::vector<std::shared_ptr<InputWidget>>& selectable);

};