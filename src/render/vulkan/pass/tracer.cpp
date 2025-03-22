
#include "tracer.hpp"
#include "render.hpp"

void RenderPassTracer::reset(RenderPass& render_pass) {
	if (active) {
		FAULT("Can't begin new render pass, another render pass bound!");
	}

	this->index = 0;
	this->count = render_pass.getSubpassCount();
	this->active = true;
}

void RenderPassTracer::advance() {
	if (!active) {
		FAULT("Can't begin new subpass, no render pass bound!");
	}

	index ++;

	if (index >= count) {
		FAULT("Unexpected subpass #", index, ", expected end of render pass!");
	}
}

void RenderPassTracer::end() {
	if (!active) {
		FAULT("Can't end render pass, no render pass bound!");
	}

	index ++;

	if (index != count) {
		throw std::runtime_error {"Unexpected end of render pass!"};
	}

	active = false;
}