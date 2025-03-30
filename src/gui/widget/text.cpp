
#include "text.hpp"

#include "render/immediate.hpp"

TextWidget::TextWidget(const std::string& text) {
	setText(text);
}

void TextWidget::draw(ImmediateRenderer& immediate) {

	immediate.setRectRadius(0);
	immediate.setColor(0, 0, 0);
	immediate.drawRect2D(content.x, content.y, content.w, content.h);
	immediate.setColor(255, 255, 255);
	immediate.drawRect2D(content.x + 2, content.y + 2, content.w - 4, content.h - 4);

	immediate.setTextAlignment(VerticalAlignment::TOP);
	immediate.setTextAlignment(HorizontalAlignment::LEFT);
	immediate.setFontSize(20);
	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	immediate.setColor(0, 0, 0);
	immediate.setTextBox(content.w, content.h);
	immediate.setWrapping(true);
	immediate.drawString2D(content.x, content.y, text.c_str());
}

bool TextWidget::event(WidgetContext& context, const InputEvent& event) {
	return false;
}

void TextWidget::setText(const std::string& text) {
	this->text = text;

	TextBakery bakary;

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

	// min_content = min.getMetrics().width;
	// max_content = max.getMetrics().width;

	width = Unit::px(unwrapped.width);
	height = Unit::px(unwrapped.height);
}


