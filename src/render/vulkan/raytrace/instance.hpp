#pragma once

#include "external.hpp"
#include "render/api/reusable.hpp"

class RenderObject;

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
