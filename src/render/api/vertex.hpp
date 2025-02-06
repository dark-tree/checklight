#pragma once

#include "external.hpp"

struct Index3D {

	using type = uint32_t;
	static constexpr VkIndexType format = VK_INDEX_TYPE_UINT32;

};

struct Vertex3D {

	public:

		float x, y, z;
		static constexpr VkFormat position = VK_FORMAT_R32G32B32_SFLOAT;

		float r, g, b;
		static constexpr VkFormat color = VK_FORMAT_R32G32B32_SFLOAT;

	public:

		Vertex3D(float x, float y, float z, float r, float g, float b);

};
