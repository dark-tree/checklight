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
#include "layout/sided.hpp"
#include "layout/radius.hpp"

class WidgetContext;
class ImmediateRenderer;
class Navigator;
class InputWidget;

class Widget : public std::enable_shared_from_this<Widget> {

	public:

		// this probably should be made protected but we need those
		// in tests so for now let it be, just pleeease don't modify those
		// fields, especially outside the tests

		/// Widget dimensions in pixels
		Sizing sizing;

		/// Widget content box with added padding
		Box2D padded { {},{},{},{} };

		/// Widget content box
		Box2D content { {},{},{},{} };

	protected:

		std::weak_ptr<Widget> parent;
		std::vector<std::shared_ptr<Widget>> children;

		Sizing minimal;
		StyleContext styling;

		/// Returns the 0.0/0.5/1.0 multiplier of the channel alignment
		float getAlignmentFactor(Channel channel);

		/// Can be called after the given channel was already computed with applyFitSizing()
		int getOuterSizing(Channel channel);

		/// Called after the on-flow dimension has ben computed, can be used to adjust content the acros-flow dimension
		virtual void applyWrapSizing();

		/// Draws the a basic panel with correct styling and sizing
		void drawBasicPanel(ImmediateRenderer& immediate, ElementState state);

	private:

		/// Propagate the styling meta-parameters to all children
		void applyStyleContext(const StyleContext& context);

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

		/// Child elements flow direction
		StyleProperty<Flow> flow = Flow::LEFT_TO_RIGHT;

		/// Vertical alignment of children elements
		StyleProperty<VerticalAlignment> vertical = VerticalAlignment::TOP;

		/// Horizontal alignment of children elements
		StyleProperty<HorizontalAlignment> horizontal = HorizontalAlignment::LEFT;

		/// Element width, can be overwritten by the layout algorithm to make the UI fit
		StyleProperty<Unit> width = Unit::fit();

		/// Element height, can be overwritten by the layout algorithm to make the UI fit
		StyleProperty<Unit> height = Unit::fit();

		/// Gap between children of this element
		StyleProperty<Unit> gap = Unit::zero();

		/// Minimal element width, layout algorithm is forced to follow this value
		StyleProperty<Unit> min_width = Unit::zero();

		/// Minimal element height, layout algorithm is forced to follow this value
		StyleProperty<Unit> min_height = Unit::zero();

		/// Margin around the element
		StyleProperty<BoxUnit> margin = BoxUnit {};

		/// Padding inside the element
		StyleProperty<BoxUnit> padding = BoxUnit {};

		/// Border width around the element
		StyleProperty<Unit> border = Unit::zero();

		/// Radius of the widget border
		StyleProperty<RadiusUnit> radius = RadiusUnit {};

		/// Widget color
		StyleProperty<Color> color = Color {};

		/// Widget color
		StyleProperty<Color> border_color = Color {};

	public:

		virtual ~Widget();
		virtual void draw(ImmediateRenderer& immediate, ElementState state) = 0;
		virtual bool event(WidgetContext& context, const InputEvent& event);
		virtual void scan(Navigator& navigator);

		/// Mark the layout for update before next frame
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

		ElementState computeWidgetState() const;

	private:

		friend class Navigator;

		/// Never invoke this method yourself!
		void setSelected(bool selected);

	public:

		virtual bool event(WidgetContext& context, const InputEvent& event);
		virtual void scan(Navigator& navigator);

};