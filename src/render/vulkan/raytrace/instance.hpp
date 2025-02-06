#pragma once

#include "external.hpp"
#include "render/api/reusable.hpp"
#include "render/vulkan/command/recorder.hpp"

class InstanceManager;

class InstanceDelegate {

	private:

		friend InstanceManager;

		VkAccelerationStructureInstanceKHR instance;
		size_t index;

	public:

		InstanceDelegate(size_t index);

		void setMatrix(const glm::mat4x3& model);
		void setShader(uint32_t index);
		void setTraits(VkGeometryInstanceFlagsKHR flags);
		void setObject();

		void flush() const;
		void close();
};

class InstanceManager {

	private:

		size_t capacity = 0;
		size_t freed = 0;

		ReusableBuffer buffer;
		std::vector<std::shared_ptr<InstanceDelegate>> delegates;

		void write(const InstanceDelegate& delegate);
		void trim();

	public:

		InstanceManager();
		~InstanceManager();

		std::shared_ptr<InstanceDelegate> create();
		void flush(RenderCommander& recorder);

		Buffer& getBuffer();

};
