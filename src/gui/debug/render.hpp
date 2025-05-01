#pragma once

#include "external.hpp"
#include "gui/gui.hpp"
#include "render/render.hpp"

class RenderSystemOverlay {

	private:

		static constexpr float max_portal_jumps = 20;
		static constexpr float max_sample_age = 1000;
		static constexpr float max_sample_count = 10;
		static constexpr float max_global_brightness = 4;

		static std::shared_ptr<PanelWidget> createOption(Theme& theme, const char* name, const std::shared_ptr<Widget>& control);

	public:

		/**
		 * Create a simple overlay that can be used to test in real time
		 * render system parameter values
		 */
		static std::shared_ptr<WidgetContext> create(Theme& theme);

};
