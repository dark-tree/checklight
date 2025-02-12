
#include "query.hpp"
#include "render/vulkan/setup/device.hpp"
#include "render/vulkan/setup/debug.hpp"

/*
 * Query
 */

uint64_t Query::get(uint64_t fallback) const {
	return present() ? value : fallback;
}

bool Query::present() const {
	return status != 0;
}

/*
 * QueryPool
 */

QueryPool::QueryPool(const LogicalDevice& device, VkQueryType type, int count, VkQueryPipelineStatisticFlags statistics) {
	VkQueryPoolCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	create_info.pNext = nullptr;
	create_info.flags = 0;

	create_info.queryCount = count;
	create_info.queryType = type;
	create_info.pipelineStatistics = statistics;

	this->length = count;
	this->vk_device = device.getHandle();

	vkCreateQueryPool(this->vk_device, &create_info, nullptr, &vk_pool);
}

void QueryPool::close() {
	if (vk_pool != VK_NULL_HANDLE) {
		vkDestroyQueryPool(vk_device, vk_pool, nullptr);
	}
}

void QueryPool::load(size_t count, void* buffer, VkQueryResultFlags flags) {
	size_t entry = sizeof(uint64_t);

	if (flags & VK_QUERY_RESULT_WITH_AVAILABILITY_BIT) {
		entry = sizeof(Query);
	}

	if (count > length) {
		throw std::runtime_error {"Incompatible query result buffer!"};
	}

	vkGetQueryPoolResults(vk_device, vk_pool, 0, count, entry * count, buffer, entry, flags | VK_QUERY_RESULT_64_BIT);
}

void QueryPool::loadAvailable(std::vector<Query>& results) {
	load(results.size(), results.data(), VK_QUERY_RESULT_WITH_AVAILABILITY_BIT);
}

void QueryPool::loadAll(std::vector<uint64_t>& results) {
	load(results.size(), results.data(), VK_QUERY_RESULT_WAIT_BIT);
}

size_t QueryPool::size() const {
	return length;
}

void QueryPool::setDebugName(const char* name) const {
	VulkanDebug::setDebugName(vk_device, VK_OBJECT_TYPE_QUERY_POOL, vk_pool, name);
}

VkQueryPool QueryPool::getHandle() const {
	return vk_pool;
}