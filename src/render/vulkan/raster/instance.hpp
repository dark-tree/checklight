#pragma once

#include "external.hpp"
#include "render/api/reusable.hpp"
#include "render/vulkan/buffer/instance.hpp"

class RenderObject;

/**
 * Manages all objects (instances) present in the scene
 */
class RasterInstanceManager final : public InstanceManager {

	protected:
		void write(const RenderObject& delegate) override;

	public:

		RasterInstanceManager();

		/// Create new delegate
		std::shared_ptr<RenderObject> create() override;
};
