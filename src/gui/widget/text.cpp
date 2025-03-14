
#include "text.hpp"

#include "render/immediate.hpp"

TextWidget::TextWidget(const std::string& text)
: text(text) {

}

void TextWidget::draw(ImmediateRenderer& immediate) {
	immediate.setColor(0, 0, 0);
	immediate.setTextBox(content.w, content.h);
	immediate.drawString2D(content.x, content.y, text.c_str());
}

bool TextWidget::event(WidgetContext& context, const InputEvent& event) {
	return false;
}

Box2D TextWidget::getInherentBox() const {
	return {0, 0, 0, 0};
}

void TextWidget::setBounds(Box2D bounds) {
	Widget::setBounds(bounds);
}


