#pragma once

#include "external.hpp"
#include "render/api/reusable.hpp"

class RenderModel;
class RenderCommander;

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
		void setModel(const std::shared_ptr<RenderModel>& model);

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
		void flush(CommandRecorder& recorder);

		const ReusableBuffer& getBuffer() const;

};
