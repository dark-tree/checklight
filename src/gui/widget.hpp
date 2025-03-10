#pragma once

#include "external.hpp"
#include "input/input.hpp"
#include "shared/box.hpp"
#include "layout/display.hpp"
#include "layout/unit.hpp"
#include "render/immediate.hpp"

class WidgetContext;
class ImmediateRenderer;

class InputWidget;

struct Spacing {
	Unit top, bottom, left, right;

	Box2D apply(int width, int height, const Box2D& initial);
	Box2D remove(int width, int height, const Box2D& initial);
};

class Widget : public std::enable_shared_from_this<Widget> {

	public: // FIXME

		Display display = Display::AUTO;
		Unit width, height;
		Spacing margin, padding;
		VerticalAlignment vertical = VerticalAlignment::TOP;
		HorizontalAlignment horizontal = HorizontalAlignment::LEFT;

		Box2D margined { {},{},{},{} }; // padded box with margin added
		Box2D padded { {},{},{},{} };   // content box with padding added
		Box2D content { {},{},{},{} };  // content box

		/// Get the actual display to use (resolve AUTO)
		Display getEffectiveDisplay() const;

		/// Return a box of free space after removing a setBounds allocation from the pool
		Box2D getRemainingBox(const Box2D& pool, const Widget* widget) const;

		virtual Box2D getInherentBox() const;
		Box2D getContentBox() const;
		Box2D getPaddingBox() const;
		Box2D getMarginBox() const;

	public:

		virtual ~Widget();
		virtual void draw(ImmediateRenderer& immediate) = 0;
		virtual bool event(WidgetContext& context, const InputEvent& event);
		virtual void appendSelectable(std::vector<std::shared_ptr<InputWidget>>& selectable);

	public:

		virtual void setBounds(Box2D bounds);

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