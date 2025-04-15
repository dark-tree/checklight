
#include "text.hpp"

#include "render/immediate.hpp"

void TextWidget::applyWrapSizing() {
	Widget::applyWrapSizing();

	TextBakery bakery = getBakery(sizing.width(), 0, ElementState::ofLayout());
	BakedText wrapped = bakery.bakeString(0, 0, text);

	min_height = Unit::px(wrapped.getMetrics().height);
}

void TextWidget::updateWidgetText(const std::string& text) {
	this->text = text;
	TextBakery bakery = getBakery(0, 0, ElementState::ofLayout());

	bakery.setWrapping(true);
	BakedText::Metrics wrapped = bakery.bakeString(0, 0, text).getMetrics();

	bakery.setWrapping(false);
	BakedText::Metrics unwrapped = bakery.bakeString(0, 0, text).getMetrics();

	// this is a sub-optimal solution
	min_width = Unit::px(wrapped.width);
	width = Unit::px(unwrapped.width);
	height = Unit::px(unwrapped.height);
}

TextBakery TextWidget::getBakery(int width, int height, const ElementState& state) const {

	TextBakery bakery;

	bakery.setFont(font.fetch(state));
	bakery.setAlignment(vertical.fetch(state));
	bakery.setAlignment(horizontal.fetch(state));
	bakery.setSize(size.fetch(state));
	bakery.setBounds(width, height);
	bakery.setWrapping(wrap.fetch(state));

	return bakery;

}

TextWidget::TextWidget()
: TextWidget("") {}

TextWidget::TextWidget(const std::string& text) {
	updateWidgetText(text);
}

void TextWidget::draw(ImmediateRenderer& immediate, ElementState state) {

	TextBakery bakery = getBakery(content.w, content.h, state);

	immediate.setFill(color.fetch(state));
	immediate.drawText2D(content.x, content.y, bakery.bakeString(0, 0, text.c_str()));

}

bool TextWidget::event(WidgetContext& context, const InputEvent& event) {
	return false;
}

void TextWidget::setText(const std::string& text) {
	updateWidgetText(text);
	update();
}


