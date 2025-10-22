#pragma once

#include "external.hpp"
#include "render/api/mesh.hpp"
#include "render/api/reusable.hpp"
#include "render/vulkan/buffer/instance.hpp"

class RenderObject;
struct RenderObjectEqual;
/**
 * Manages all objects (instances) present in the scene
 */
class RasterInstanceManager final : public InstanceManager {
	private:
		void sortDelegates(const std::map<std::shared_ptr<RenderMesh>, void*, RenderObjectEqual>& object_meshes);
	protected:
		bool modifyDelegateVector=true;
		void write(const RenderObject& delegate) override;

	public:

		RasterInstanceManager();

		/// Create new delegate
		std::shared_ptr<RenderObject> create() override;
		void flush(CommandRecorder& recorder,const std::map<std::shared_ptr<RenderMesh>, void*, RenderObjectEqual>& object_meshes);
};
