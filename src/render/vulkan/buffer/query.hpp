#pragma once

#include "external.hpp"

class LogicalDevice;

struct Query {

	uint64_t value;
	uint64_t status;

	/**
	 * Returns the query value or the provided fallback default
	 * value if the query result is not available
	 */
	uint64_t get(uint64_t fallback) const;

	/**
	 * Verify that this method returns true before trying to
	 * read the value returned by the query read (.value)
	 */
	bool present() const;

};

static_assert(sizeof(Query) == 2 * sizeof(uint64_t));

/**
 * For information regarding Vulkan Timestamps see
 * https://nikitablack.github.io/post/how_to_use_vulkan_timestamp_queries/
 */
class QueryPool {

	private:

		std::vector<Query> results;

	public:

		VkDevice vk_device;
		VkQueryPool vk_pool;

	public:

		QueryPool() = default;
		QueryPool(LogicalDevice& device, VkQueryType type, int count, VkQueryPipelineStatisticFlags statistics = 0);

		/// Free the underlying vulkan object
		void close();

		/// Copies all the queries into a local buffer
		void load();

		/// Return the specified query
		Query read(int index) const;

		/// Get the number of queries in this pool
		size_t size() const;

		/// Sets a human readable name visible in external debuggers
		void setDebugName(const char* name) const;

};
