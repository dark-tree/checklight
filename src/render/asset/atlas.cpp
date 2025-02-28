
#include "atlas.hpp"
#include "render/api/commander.hpp"
#include "render/system.hpp"
#include "sprite.hpp"

/*
 * DynamicAtlas
 */

Sprite DynamicAtlas::packSprite(ImageData& image) {

	Box2D box = Box2D::allocate(this->pool, image.width(), image.height());

	if (!box.empty()) {
		atlas.blit(box.x, box.y, image);
		return {box.x, box.y, box.x + box.w, box.y + box.h};
	}

	int w = atlas.width();
	int h = atlas.width();

	// retry with bigger atlas
	pool.emplace_back(w, 0, w, h);
	pool.emplace_back(0, h, 2 * w, h);
	atlas.resize(w + w, h + h);
	resized = true;

	return packSprite(image);
}

void DynamicAtlas::resize() {
	auto commander = RenderSystem::system->createTransientCommander();

	ManagedImageDataSet set {atlas, false};
	image = set.upload(RenderSystem::system->allocator, commander->getRecorder(), VK_FORMAT_R8G8B8A8_SRGB);

	commander->getTaskQueue().enqueue([texture = texture] () mutable {
		texture.closeImageViewSampler(RenderSystem::system->device.getHandle());
	});

	texture = TextureBuilder::begin()
		.setType(VK_IMAGE_VIEW_TYPE_2D)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setFilter(VK_FILTER_LINEAR)
		.setDebugName("Dynamic Atlas Image")
		.createTexture(RenderSystem::system->device, image.getImage());

	rewrite = false;
	resized = false;
	commander->complete();
	printf("INFO: Dynamic atlas was resized!\n");
}

DynamicAtlas::DynamicAtlas() {
	atlas = ImageData::allocate(16, 16);
	pool.emplace_back(0, 0, 16, 16);
}

void DynamicAtlas::close(const LogicalDevice& device) {
	atlas.close();
	texture.closeViewSampler(device.getHandle());
	image.close();
}

Sprite DynamicAtlas::submit(ImageData& image) {
	rewrite = true;
	return packSprite(image);
}

void DynamicAtlas::upload(CommandRecorder& recorder) {

	// atlas run out of space and resized itself
	if (resized) {
		resize();
	}

	// something changed
	if (rewrite) {
		image.write(0, atlas, atlas.width(), atlas.height(), 0);
		image.upload(recorder);

		rewrite = false;
		printf("INFO: Dynamic atlas was uploaded!\n");
	}

}

Texture& DynamicAtlas::getTexture() {
	return texture;
}

/*
 * DynamicImageAtlas
 */

DynamicImageAtlas::DynamicImageAtlas(const std::shared_ptr<DynamicAtlas>& atlas)
: atlas(atlas) {

	ImageData blank = ImageData::allocate(16, 16);
	blank.clear({255, 255, 255, 255});
	sprites[BLANK_SPRITE] = atlas->submit(blank);
	blank.close();

}

Sprite DynamicImageAtlas::getOrLoad(const std::string& path) {
	auto it = sprites.find(path);

	if (it != sprites.end()) {
		return it->second;
	}

	ImageData data = ImageData::loadFromFile(path, 4);
	Sprite sprite = atlas->submit(data);
	data.close();

	sprites[path] = sprite;
	return sprite;
}