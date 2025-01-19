#pragma once

#include "external.hpp"

class RenderPass;

class RenderPassTracer {

	private:

		bool active = false;
		int index;
		int count;

	public:

		void reset(RenderPass& render_pass);
		void advance();
		void end();

};