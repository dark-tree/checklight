#pragma once

#include "external.hpp"
#include "atlas.hpp"
#include "font.hpp"

/**
 * This class is used for loading of external assets
 * that are to be used during rendering
 */
class AssetLoader {

	private:

		friend class Renderer;

		std::shared_ptr<DynamicAtlas> atlas;
		DynamicImageAtlas images;
		DynamicFontAtlas fonts;

		AssetLoader();

	public:

		/**
		 * Get sprite of image at the given path, or throw.
		 * The returned image is cached and not re-loaded if requested again.
		 */
		Sprite getSprite(const std::string& path);

		/**
		 * Get empty (non-trasparent all-white) image, usefull if texture is not desired.
		 * The returned image is cached and not re-loaded if requested again.
		 */
		Sprite getBlankSprite();

		/**
		 * Get font loaded from .TTF file at the given path, or throw.
		 * The returned font is cached and not re-loaded if requested again.
		 */
		std::shared_ptr<Font> getFont(const std::string& path);

		/**
		 * Get the underlying image/font atlas, this atlas is managed
		 * by the immediate render system!
		 */
		std::shared_ptr<DynamicAtlas> getSharedAtlas();

};
