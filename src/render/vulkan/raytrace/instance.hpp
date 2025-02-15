#pragma once

#include "external.hpp"
#include "render/api/reusable.hpp"

class RenderObject;

/**
 * Manages all objects (instances) present in the scene
 */
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

		/// Create new delegate
		std::shared_ptr<RenderObject> create();

		/// Write all delegates to the staging buffer
		void flush(CommandRecorder& recorder);

		/// Get the buffer object used for instance array storage
		const ReusableBuffer& getBuffer() const;

};
