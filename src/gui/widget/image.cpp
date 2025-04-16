#include "image.hpp"

ImageWidget::ImageWidget()
: ImageWidget("") {

}

ImageWidget::ImageWidget(const std::string& sprite) {
	setSprite(sprite);
}

void ImageWidget::draw(ImmediateRenderer& immediate, ElementState state) {

	drawBasicPanel(immediate, state);

	if (!sprite.empty()) {
		immediate.setSprite(sprite);
		immediate.drawRect2D(padded);
	}

}

void ImageWidget::setSprite(const std::string& sprite) {
	this->sprite = sprite;
}
