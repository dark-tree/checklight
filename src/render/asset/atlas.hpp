#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/image.hpp"
#include "shared/box.hpp"
#include "render/vulkan/buffer/texture.hpp"
#include "sprite.hpp"

class DynamicAtlas {

	private:

		bool rewrite = true;
		bool resized = true;

		MutableImage image;
		Texture texture;

		ImageData atlas;
		std::list<Box2D> pool;

		Sprite packSprite(ImageData& image);
		void resize();

	public:

		DynamicAtlas();

		/// Close the underlying resources
		void close(const LogicalDevice& device);

		/// Get the image data containing all submitted images
		ImageData& getImage();

		/// Add image (with margin) to atlas and return its coordinates
		Sprite submitWithMargin(ImageData& image);

		/// Upload (or recreate) underlying vulkan texture
		bool upload(CommandRecorder& recorder);

		/// Get underlying vulkan texture
		Texture& getTexture();

};

class DynamicImageAtlas {

	private:

		std::shared_ptr<DynamicAtlas> atlas;
		std::unordered_map<std::string, Sprite> sprites;

	public:

		static constexpr const char* BLANK_SPRITE = ":blank";

		DynamicImageAtlas() = default;
		DynamicImageAtlas(const std::shared_ptr<DynamicAtlas>& atlas);

		Sprite getOrLoad(const std::string& path);

};
