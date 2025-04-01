
#include "text.hpp"

#include "render/immediate.hpp"

void TextWidget::applyWrapSizing() {
	Widget::applyWrapSizing();

	TextBakery bakary;

	// todo
	bakary.setFont("assets/font/OpenSans-Variable.ttf");
	bakary.setAlignment(VerticalAlignment::TOP);
	bakary.setAlignment(HorizontalAlignment::LEFT);
	bakary.setSize(20);

	bakary.setBounds(sizing.width(), 0);
	bakary.setWrapping(true);
	BakedText wrapped = bakary.bakeString(0, 0, text);

	height = Unit::px(wrapped.getMetrics().height);
}

TextWidget::TextWidget(const std::string& text) {
	setText(text);
}

void TextWidget::draw(ImmediateRenderer& immediate) {

	immediate.setRectRadius(0);
	immediate.setColor(0, 0, 0);
	immediate.drawRect2D(content.x, content.y, content.w, content.h);
	immediate.setColor(255, 255, 255);
	immediate.drawRect2D(content.x + 2, content.y + 2, content.w - 4, content.h - 4);

	immediate.setColor(255, 0, 0);
	immediate.drawRect2D(content.x, content.y, minimal.width(), 4);

	// todo
	immediate.setTextAlignment(VerticalAlignment::TOP);
	immediate.setTextAlignment(HorizontalAlignment::LEFT);
	immediate.setFontSize(20);
	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	immediate.setColor(0, 0, 0);
	immediate.setTextBox(content.w, content.h);
	immediate.setWrapping(true);
	immediate.drawString2D(content.x, content.y, text.c_str());

	immediate.setColor(255, 0, 0);
	immediate.drawString2D(content.x, content.y + sizing.height(), std::to_string(minimal.width()));
}

bool TextWidget::event(WidgetContext& context, const InputEvent& event) {
	return false;
}

void TextWidget::setText(const std::string& text) {
	this->text = text;

	TextBakery bakary;

	// todo
	bakary.setFont("assets/font/OpenSans-Variable.ttf");
	bakary.setAlignment(VerticalAlignment::TOP);
	bakary.setAlignment(HorizontalAlignment::LEFT);
	bakary.setSize(20);

	bakary.setBounds(0, 0);
	bakary.setWrapping(true);
	BakedText::Metrics wrapped = bakary.bakeString(0, 0, text).getMetrics();

	bakary.setBounds(0, 0);
	bakary.setWrapping(false);
	BakedText::Metrics unwrapped = bakary.bakeString(0, 0, text).getMetrics();

	min_width = Unit::px(wrapped.width);

	width = Unit::px(unwrapped.width);
	height = Unit::px(unwrapped.height);
}


