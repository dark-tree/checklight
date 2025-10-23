
#include "instance.hpp"

#include "render/api/model.hpp"
#include "shared/math.hpp"
#include "render/api/object.hpp"

/*
 * RasterInstanceManager
 */

void RasterInstanceManager::sortDelegates(const std::map<std::shared_ptr<RenderMesh>, void*, RenderObjectEqual>& object_meshes) {
	std::vector<std::shared_ptr<RenderObject>> sorted_delegates;

	uint32_t index=0;
	for (const auto& mesh: object_meshes) {
		for (auto& delegate: delegates) {
			if (delegate->getModel()->getMesh() == mesh.first) {
				delegate->setIndex(index);
				sorted_delegates.push_back(delegate);
				index++;
			}
		}
	}

	delegates = std::move(sorted_delegates);
}

void RasterInstanceManager::flush(CommandRecorder& recorder,const std::map<std::shared_ptr<RenderMesh>, void*, RenderObjectEqual>& object_meshes) {

	if (modifyDelegateVector) {
		sortDelegates(object_meshes);
		modifyDelegateVector=false;
	}

	for (const std::shared_ptr<RenderObject>& delegate : delegates) {
		write(*delegate);
	}

	instance_buffer.flushStaging(recorder);
	attachment_buffer.flushStaging(recorder);
}

void RasterInstanceManager::write(const RenderObject& delegate) {
	instance_buffer.writeToStaging(delegate.getRasterInstanceData(), 1, sizeof(RasterInstanceData), delegate.getIndex());
	attachment_buffer.writeToStaging(delegate.getObjectData(), 1, sizeof(RenderObjectData), delegate.getIndex());
}

RasterInstanceManager::RasterInstanceManager()
	: InstanceManager(
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
	){};

std::shared_ptr<RenderObject> RasterInstanceManager::create() {

	// maybe not the best idea, but we can change it later
	if (freed == 0) {
		trim();
	}

	modifyDelegateVector=true;
	// no blocks to reuse
	if (freed == 0) {
		size_t offset = delegates.size();

		// no space in buffer left
		if (offset >= capacity) {

			capacity = (capacity > 0) ? (capacity * 2) : 16;

			instance_buffer.close();
			instance_buffer.allocateBuffers(capacity, sizeof(RasterInstanceData));

			attachment_buffer.close();
			attachment_buffer.allocateBuffers(capacity, sizeof(RenderObjectData));
		}

		return delegates.emplace_back(std::make_shared<RenderObject>(offset));
	}

	freed --;
	const auto it = std::find(delegates.begin(), delegates.end(), nullptr);
	const size_t offset = std::distance(delegates.begin(), it);

	auto delegate = std::make_shared<RenderObject>(offset);
	(*it) = delegate;

	return delegate;
}