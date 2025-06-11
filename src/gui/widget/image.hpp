#pragma once

#include "gui/widget.hpp"

/**
 * Simple sprite box widget
 */
class ImageWidget : public Widget {

	private:

		std::string sprite;

	public:

		ImageWidget();
		ImageWidget(const std::string& sprite);

		void draw(ImmediateRenderer& immediate, ElementState state) override;

		void setSprite(const std::string& sprite);

};