#pragma once

#include "external.hpp"
#include "input/input.hpp"
#include "shared/box.hpp"
#include "layout/flow.hpp"
#include "layout/sizing.hpp"
#include "layout/unit.hpp"
#include "render/immediate.hpp"
#include "layout/state.hpp"
#include "layout/property.hpp"

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

		int getTotal(const StyleContext& styling, Channel channel);

};

class Widget : public std::enable_shared_from_this<Widget> {

	public: // FIXME

		std::weak_ptr<Widget> parent;
		std::vector<std::shared_ptr<Widget>> children;

		Sizing sizing;
		Sizing minimal;

		StyleContext styling;
		StyleProperty<Flow> flow = Flow::LEFT_TO_RIGHT;
		StyleProperty<VerticalAlignment> vertical = VerticalAlignment::TOP;
		StyleProperty<HorizontalAlignment> horizontal = HorizontalAlignment::LEFT;
		StyleProperty<Unit> width = Unit::fit();
		StyleProperty<Unit> height = Unit::fit();
		StyleProperty<Unit> gap = Unit::zero();
		StyleProperty<Unit> min_width = Unit::zero();
		StyleProperty<Unit> min_height = Unit::zero();
		StyleProperty<Spacing> margin {Spacing {}};
		StyleProperty<Spacing> padding {Spacing {}};

		Box2D padded { {},{},{},{} };   // content box with added padding
		Box2D content { {},{},{},{} };  // content box

		float getAlignmentFactor(Channel channel);

		/// Can be called after the given channel was already computed with applyFitSizing()
		int getOuterSizing(Channel channel);

		/// Called after the on-flow dimension has ben computed, can be used to adjust content the acros-flow dimension
		virtual void applyWrapSizing();

		/// Propagate the styling meta-parameters to all children
		void applyStyleContex(const StyleContext& context);

		/// Compute element size based on its children (absolute & fit content)
		void applyFitSizing(Channel channel);

		/// Adjust element's children sizes to fill remaining space
		void applyGrowSizing(Channel channel);

		/// Create final binding boxes based on sizing information
		void applyPositioning(int x, int y);

	protected:

		void rebuild(int x, int y);
		void add(const std::shared_ptr<Widget>& child);
		void remove(const std::shared_ptr<Widget>& child);

	public:

		virtual ~Widget();
		virtual void draw(ImmediateRenderer& immediate, ElementState state) = 0;
		virtual bool event(WidgetContext& context, const InputEvent& event);
		virtual void scan(Navigator& navigator);
		virtual void update();

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