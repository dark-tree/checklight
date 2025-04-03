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
class Navigator;
class InputWidget;

struct Spacing {
	Unit top, bottom, left, right;

	public:

		Spacing()
		: top(Unit::zero()), bottom(Unit::zero()), left(Unit::zero()), right(Unit::zero()) {}

		Spacing(Unit unit)
		: top(unit), bottom(unit), left(unit), right(unit) {}

		Spacing(Unit vertical, Unit horizontal)
		: top(vertical), bottom(vertical), left(horizontal), right(horizontal) {}

		Spacing(Unit top, Unit bottom, Unit left, Unit right)
		: top(top), bottom(bottom), left(left), right(right) {}

	public:

		int getTotal(Channel channel);

};

class Widget : public std::enable_shared_from_this<Widget> {

	public: // FIXME

		std::vector<std::shared_ptr<Widget>> children;
		Sizing sizing;
		Sizing minimal;

		Flow flow = Flow::LEFT_TO_RIGHT;
		Unit width, height, gap, min_width, min_height;
		Spacing margin, padding;

		VerticalAlignment vertical = VerticalAlignment::TOP;
		HorizontalAlignment horizontal = HorizontalAlignment::LEFT;

		Box2D padded { {},{},{},{} };   // content box with padding added
		Box2D content { {},{},{},{} };  // content box

		Box2D getContentBox() const;
		Box2D getPaddingBox() const;

		float getAlignmentFactor(Channel channel);

		/// Can be called after the given channel was already computed with applyFitSizing()
		int getOuterSizing(Channel channel);

		/// Called after the on-flow dimension has ben computed, can be used to adjust content the acros-flow dimension
		virtual void applyWrapSizing();

		/// Compute element size based on its children (absolute & fit content)
		void applyFitSizing(Channel channel);

		/// Adjust element's children sizes to fill remaining space
		void applyGrowSizing(Channel channel);

		/// Create final binding boxes based on sizing information
		void applyPositioning(int x, int y);

	public:

		virtual ~Widget();
		virtual void draw(ImmediateRenderer& immediate) = 0;
		virtual bool event(WidgetContext& context, const InputEvent& event);
		virtual void scan(Navigator& navigator);

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

		friend class Navigator;

		/// Never invoke this method yourself!
		void setSelected(bool selected);

	public:

		virtual bool event(WidgetContext& context, const InputEvent& event);
		virtual void scan(Navigator& navigator);

};