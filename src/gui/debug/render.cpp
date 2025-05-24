#include "render.hpp"

/*
 * RenderSystemOverlay
 */

std::shared_ptr<PanelWidget> RenderSystemOverlay::createOption(Theme& theme, const char* name, const std::shared_ptr<Widget>& control) {
	auto option = theme.newPanel();
	option->flow = Flow::LEFT_TO_RIGHT;
	option->width = Unit::grow();

	auto label = theme.newText(name);
	label->vertical = VerticalAlignment::CENTER;
	label->horizontal = HorizontalAlignment::CENTER;
	label->width = Unit::grow();
	label->height = Unit::grow();

	option->addWidget(label);
	option->addWidget(control);

	return option;
}

std::shared_ptr<WidgetContext> RenderSystemOverlay::create(Theme& theme) {

	RenderSystem& system = *RenderSystem::system;
	RenderParameters& parameters = system.getParameters();

	// create root panel
	auto context = std::make_shared<WidgetContext>();
	auto panel = theme.newRoot(15, 15);

	auto settings = theme.newPanel();
	settings->flow = Flow::TOP_TO_BOTTOM;
	settings->gap = Unit::px(10);
	settings->width = Unit::px(250);
	settings->padding = Unit::px(10);

	panel->addWidget(settings);

	{
		auto control = theme.newSlider(parameters.getPortalMaxJumps() / max_portal_jumps, 1.0/max_portal_jumps);
		control->width = Unit::grow();

		control->onChange([] (float value) {
			RenderSystem::system->getParameters().setPortalMaxJumps(value * max_portal_jumps);
		});

		settings->addWidget(createOption(theme, "Portal Depth", control));
	}

	{
		auto control = theme.newSlider(parameters.getSkyEmission() / 100);
		control->width = Unit::grow();

		control->onChange([] (float value) {
			RenderSystem::system->getParameters().setSkyEmission(value * 100);
		});

		settings->addWidget(createOption(theme, "Sky Emission", control));
	}

	{
		auto control = theme.newSlider(parameters.getGIBrightness() / max_global_brightness);
		control->width = Unit::grow();

		control->onChange([] (float value) {
			RenderSystem::system->getParameters().setGIBrightness(value * max_global_brightness);
		});

		settings->addWidget(createOption(theme, "Brightness", control));
	}

	{
		auto control = theme.newSlider(parameters.getMaxSampleAge() / max_sample_age);
		control->width = Unit::grow();

		control->onChange([] (float value) {
			RenderSystem::system->getParameters().setMaxSampleAge(value * max_sample_age);
		});

		settings->addWidget(createOption(theme, "Age Limit", control));
	}

	{
		auto control = theme.newSlider(parameters.getGISamples() / max_sample_count, 1.0f/ max_sample_count);
		control->width = Unit::grow();

		control->onChange([] (float value) {
			RenderSystem::system->getParameters().setGISamples(value * max_sample_count);
		});

		settings->addWidget(createOption(theme, "Samples", control));
	}

	{
		auto control = theme.newSelect({"Disabled", "Enabled"});
		control->setSelected(parameters.getShadows());
		control->width = Unit::grow();

		control->onSelect([] (int index) {
			RenderSystem::system->getParameters().setShadows((bool) index);
		});

		settings->addWidget(createOption(theme, "Shadows", control));
	}

	{
		auto control = theme.newSelect({"Disabled", "Enabled"});
		control->setSelected(parameters.getDenoise());
		control->width = Unit::grow();

		control->onSelect([] (int index) {
			RenderSystem::system->getParameters().setDenoise((bool) index);
		});

		settings->addWidget(createOption(theme, "Denoise", control));
	}

	{
		auto control = theme.newSelect({"Disabled", "Enabled"});
		control->setSelected(parameters.getPortalGIEnable());
		control->width = Unit::grow();

		control->onSelect([] (int index) {
			RenderSystem::system->getParameters().setPortalGIEnable((bool) index);
		});

		settings->addWidget(createOption(theme, "Portal Lighting", control));
	}

	auto info = theme.newText("Press ESC to free the cursor");
	info->width = Unit::grow();
	info->horizontal = HorizontalAlignment::CENTER;
	info->margin = BoxUnit {Unit::px(10), Unit::zero(), Unit::zero(), Unit::zero()};
	info->color = {50, 50, 50};
	settings->addWidget(info);

	context->setRoot(panel);
	return context;

}
