#pragma once

#include "external.hpp"
#include "render/api/reusable.hpp"
#include "render/vulkan/command/recorder.hpp"

class RenderObject {

	private:

		uint32_t index;

	protected:

		VkAccelerationStructureInstanceKHR instance;

	public:

		RenderObject(uint32_t index);

		const VkAccelerationStructureInstanceKHR* getData() const;
		uint32_t getIndex() const;

		void setMatrix(const glm::mat4x3& model);
		void setShader(uint32_t index);
		void setTraits(VkGeometryInstanceFlagsKHR flags);
		void setObject();

};

class InstanceManager {

	private:

		size_t capacity = 0;
		size_t freed = 0;

		ReusableBuffer buffer;
		std::vector<std::shared_ptr<RenderObject>> delegates;

		void write(const RenderObject& delegate);
		void trim();

	public:

		InstanceManager();
		~InstanceManager();

		std::shared_ptr<RenderObject> create();
		void flush(RenderCommander& recorder);

		Buffer& getBuffer();

};
