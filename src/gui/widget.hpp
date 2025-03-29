#pragma once

#include "external.hpp"
#include "input/input.hpp"
#include "shared/box.hpp"
#include "layout/flow.hpp"
#include "layout/sizing.hpp"
#include "layout/unit.hpp"
#include "render/immediate.hpp"

class WidgetContext;
class ImmediateRenderer;

class InputWidget;

struct Spacing {
	Unit top, bottom, left, right;

	public:

		Spacing()
		: top(Unit::zero()), bottom(Unit::zero()), left(Unit::zero()), right(Unit::zero()) {}

		Spacing(Unit unit)
		: top(unit), bottom(unit), left(unit), right(unit) {}

		Spacing(Unit top, Unit bottom, Unit left, Unit right)
		: top(top), bottom(bottom), left(left), right(right) {}

};

class Widget : public std::enable_shared_from_this<Widget> {

	public: // FIXME

		std::vector<std::shared_ptr<Widget>> children;
		Sizing sizing;

		Flow flow = Flow::LEFT_TO_RIGHT;
		Unit width, height, gap;
		Spacing margin, padding;

		// VerticalAlignment vertical = VerticalAlignment::TOP;
		// HorizontalAlignment horizontal = HorizontalAlignment::LEFT;

		Box2D margined { {},{},{},{} }; // padded box with margin added
		Box2D padded { {},{},{},{} };   // content box with padding added
		Box2D content { {},{},{},{} };  // content box

		virtual Box2D getInherentBox() const;
		Box2D getContentBox() const;
		Box2D getPaddingBox() const;
		Box2D getMarginBox() const;

		/// Can be called after the given channel was already computed with applyFitSizing()
		int getOuterSizing(Channel channel);

		/// Compute element size based on its children (absolute & fit content)
		void applyFitSizing(Channel channel);

		void applyGrowSizing(Channel channel, int parent);

		/// Create final binding boxes based on sizing information
		void applyPositioning(int x, int y);

	public:

		virtual ~Widget();
		virtual void draw(ImmediateRenderer& immediate) = 0;
		virtual bool event(WidgetContext& context, const InputEvent& event);
		virtual void appendSelectable(std::vector<std::shared_ptr<InputWidget>>& selectable);

	public:

		void rebuild(int x, int y);

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