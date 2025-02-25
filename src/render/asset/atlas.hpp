#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/image.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "shared/box.hpp"
#include "render/vulkan/buffer/texture.hpp"

struct Sprite;

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
		
		void close(const LogicalDevice& device);

		ImageData& getImage();
		Sprite submit(ImageData& image);

		void upload(CommandRecorder& recorder);
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
