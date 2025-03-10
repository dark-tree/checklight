#pragma once

#include "external.hpp"
#include "input/input.hpp"
#include "shared/box.hpp"

class WidgetContext;
class ImmediateRenderer;

class InputWidget;

struct Spacing {
	int top, bottom, left, right;
};

class Widget : public std::enable_shared_from_this<Widget> {

	protected:

		Box2D padded { 0,0,0,0 };  // content box with padding added
		Box2D content { 0,0,0,0 }; // content box

		Box2D getContentBox() const;
		Box2D getPaddingBox() const;
		Box2D getMarginBox() const;

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