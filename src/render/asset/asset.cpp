#include "asset.hpp"

/*
 * AssetLoader
 */

AssetLoader::AssetLoader()
: atlas(std::make_shared<DynamicAtlas>()), images(atlas), fonts(atlas) {}

Sprite AssetLoader::getSprite(const std::string& path) {
	return images.getOrLoad(path);
}

Sprite AssetLoader::getBlankSprite() {
	return images.getOrLoad(DynamicImageAtlas::BLANK_SPRITE);
}

std::shared_ptr<Font> AssetLoader::getFont(const std::string& path) {
	return fonts.getOrLoad(path);
}

std::shared_ptr<DynamicAtlas> AssetLoader::getSharedAtlas() {
	return atlas;
}
