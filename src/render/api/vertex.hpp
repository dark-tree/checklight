#pragma once

#include "external.hpp"

struct Index3D {

	/// vertex index
	using type = uint32_t;
	static constexpr VkIndexType format = VK_INDEX_TYPE_UINT32;

};

struct Vertex3D {

	public:

		/// vertex position
		float x, y, z;
		static constexpr VkFormat position = VK_FORMAT_R32G32B32_SFLOAT;

		/// vertex color
		uint8_t r, g, b, a;
		static constexpr VkFormat color = VK_FORMAT_R8G8B8A8_UNORM;

		/// vertex texture
		float u, v;
		static constexpr VkFormat texture = VK_FORMAT_R32G32_SFLOAT;

	public:

		Vertex3D(float x, float y, float z, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

};
