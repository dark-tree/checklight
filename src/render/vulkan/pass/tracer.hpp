#pragma once

#include "external.hpp"

class RenderPass;

/**
 * This class exists to validate vulkan API usage during command buffer recording.
 * It tracks the usage of render passes and their subpasses and raises an exception
 * (std::runtime_error) if the tracked usage is invalid.
 */
class RenderPassTracer {

	private:

		bool active = false;
		int index;
		int count;

	public:

		/// Validates the 'renderpass begin' operation
		void reset(RenderPass& render_pass);

		/// Validates the 'next subpass' operation
		void advance();

		/// Validates the 'end renderpass' operation
		void end();

};