
#include "text.hpp"

#include "render/immediate.hpp"

void TextWidget::applyWrapSizing() {
	Widget::applyWrapSizing();

	TextBakery bakery = getBakery(sizing.width(), 0);
	BakedText wrapped = bakery.bakeString(0, 0, text);

	min_height = Unit::px(wrapped.getMetrics().height);
}

void TextWidget::updateWidgetText(const std::string& text) {
	this->text = text;
	TextBakery bakery = getBakery(0, 0);

	bakery.setWrapping(true);
	BakedText::Metrics wrapped = bakery.bakeString(0, 0, text).getMetrics();

	bakery.setWrapping(false);
	BakedText::Metrics unwrapped = bakery.bakeString(0, 0, text).getMetrics();

	min_width = Unit::px(wrapped.width);

	width = Unit::px(unwrapped.width);
	height = Unit::px(unwrapped.height);
}

TextBakery TextWidget::getBakery(int width, int height) const {

	TextBakery bakery;

	bakery.setFont("assets/font/OpenSans-Variable.ttf");
	bakery.setAlignment(VerticalAlignment::TOP);
	bakery.setAlignment(HorizontalAlignment::LEFT);
	bakery.setSize(20);
	bakery.setBounds(width, height);
	bakery.setWrapping(true);

	return bakery;

}

TextWidget::TextWidget() {
	updateWidgetText("");
}

TextWidget::TextWidget(const std::string& text) {
	updateWidgetText(text);
}

void TextWidget::draw(ImmediateRenderer& immediate, ElementState state) {

	// immediate.setRectRadius(0);
	// immediate.setColor(0, 0, 0);
	// immediate.drawRect2D(content.x, content.y, content.w, content.h);
	// immediate.setColor(255, 255, 255);
	// immediate.drawRect2D(content.x + 2, content.y + 2, content.w - 4, content.h - 4);

	TextBakery bakery = getBakery(content.w, content.h);

	immediate.setFill(0, 0, 0);
	immediate.drawText2D(content.x, content.y, bakery.bakeString(0, 0, text.c_str()));

}

bool TextWidget::event(WidgetContext& context, const InputEvent& event) {
	return false;
}

void TextWidget::setText(const std::string& text) {
	updateWidgetText(text);
	update();
}


