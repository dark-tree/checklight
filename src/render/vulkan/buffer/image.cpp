
#include "image.hpp"
#include "buffer.hpp"
#include <shared/file.hpp>

/*
 * ImageData
 */

ImageData::ImageData(Type type, void* pixels, int w, int h, int c)
: type(type), pixels(pixels), w(w), h(h), c(c) {}

void ImageData::resize(int w, int h) {
	if (w * h * channels() < size()) {
		return;
	}

	ImageData buffer = ImageData::allocate(w, h, channels());
	buffer.blit(0, 0, *this);

	// no matter how our image was allocated, free it now
	close();

	this->pixels = buffer.pixels;
	this->w = w;
	this->h = h;
}

ImageData ImageData::scaled(ImageScaling scaling) {

	int sw = std::max(1, w / 2);
	int sh = std::max(1, h / 2);

	ImageData scaled = ImageData::allocate(sw, sh, channels());

	if (scaling == ImageScaling::IGNORE_CONTENT) {
		return scaled;
	}

	if (scaling == ImageScaling::NEAREST) {
		for (int y = 0; y < sh; y ++) {
			for (int x = 0; x < sw; x ++) {
				memcpy(scaled.pixel(x, y), pixel(x * 2, y * 2), channels());
			}
		}

		return scaled;
	}

	UNREACHABLE;

}

void ImageData::blit(int ox, int oy, ImageData image) {
	if (ox + image.width() > w || oy + image.height() > h) {
		throw std::runtime_error {"Can't blit-in the given image, invalid placement!"};
	}

	if (image.channels() != channels()) {
		throw std::runtime_error {"Can't blit-in the given image, invalid channel count!"};
	}

	for (int y = 0; y < (int) image.height(); y ++) {
		memcpy(pixel(ox, oy + y), image.pixel(0, y), image.width() * image.channels());
	}
}

uint8_t* ImageData::pixel(int x, int y) {
	return ((uint8_t*) pixels) + (x + y * w) * channels();
}

void ImageData::close() {
	if (pixels != nullptr) {
		if (type == STB_IMAGE) stbi_image_free(pixels);
		if (type == MALLOCED) free(pixels);
	}
}

void ImageData::clear(std::initializer_list<uint8_t> value) {
	if ((int) value.size() != channels()) {
		throw std::runtime_error {"Can't clear image with given value, invalid channel count!"};
	}

	for (int y = 0; y < (int) h; y ++) {
		for (int x = 0; x < (int) w; x++) {
			memcpy(pixel(x, y), value.begin(), c);
		}
	}
}

void ImageData::save(const std::string& path) const {
	file::createPathDirectories(path);

	if (!stbi_write_png(path.c_str(), w, h, c, pixels, w * c)) {
		FAULT("Failed to save image '", path, "'");
	}
}

ImageData ImageData::expand(int margin) {
	if (margin < 0) {
		FAULT("Invalid image margin ", margin, "!");
	}

	ImageData result = ImageData::allocate(w + margin * 2, h + margin * 2, channels());
	result.blit(margin, margin, *this);

	// short-circuit when no margin
	if (margin == 0) {
		return result;
	}

	for (int i = 1; i <= margin; i ++) {

		int start = margin - i;
		int end = margin + i;
		int last = end - 1;

		for (int y = start; y < (h + end); y ++) {
			memcpy(result.pixel(start, y), result.pixel(start + 1, y), channels());
			memcpy(result.pixel(w + last, y), result.pixel(w + last - 1, y), channels());
		}

		for (int x = start; x < (w + end); x ++) {
			memcpy(result.pixel(x, start), result.pixel(x, start + 1), channels());
			memcpy(result.pixel(x, h + last), result.pixel(x, h + last - 1), channels());
		}
	}

	return result;
}

ImageData ImageData::loadFromFile(const std::string& path, int channels) {
	int ignored, w, h;
	void* pixels = stbi_load(path.c_str(), &w, &h, &ignored, channels);

	if (!pixels) {
		FAULT("Failed to load image from '" + path + "'");
	}

	return {Type::STB_IMAGE, pixels, w, h, channels};
}

ImageData ImageData::allocate(int w, int h, int channels) {
	return {Type::MALLOCED, calloc(w * h * channels, 1), w, h, channels};
}

ImageData ImageData::view(void* pixels, int w, int h, int channels) {
	return {Type::VIEW, pixels, w, h, channels};
}

Image ImageData::upload(Allocator& allocator, CommandRecorder& recorder, TaskQueue& queue, VkFormat format, bool mipmaps) const {
	ManagedImageDataSet set {*this, mipmaps};
	Image image = set.upload(allocator, recorder, format).discardStaging(queue);

	// delete mipmaps
	for (int i = 1; i < set.levels(); i ++) {
		set.level(i).close();
	}

	return image;
}

int ImageData::size() const {
	return width() * height() * channels();
}

int ImageData::width() const {
	return w;
}

int ImageData::height() const {
	return h;
}

int ImageData::channels() const {
	return c;
}

const void* ImageData::data() const {
	return pixels;
}

/*
 * ManagedImageDataSet
 */

void ManagedImageDataSet::resizeImages(int ws, int hs) {
	for (ImageData& image : images) {
		image.resize(ws * image.width(), hs * image.height());
	}
}

ManagedImageDataSet::ManagedImageDataSet(int w, int h, int channels, bool mipmaps)
: ManagedImageDataSet(ImageData::allocate(w, h, channels), mipmaps) {}

ManagedImageDataSet::ManagedImageDataSet(ImageData image, bool mipmaps) {
	images.push_back(image);
	height = image.height();
	layer = 0;

	if (mipmaps) {
		while (image.width() != 1 || image.height() != 1) {
			image = image.scaled(ImageScaling::IGNORE_CONTENT);
			images.push_back(image);
		}
	}
}

int ManagedImageDataSet::levels() const {
	return images.size();
}

int ManagedImageDataSet::layers() const {
	return std::max(1, layer);
}

ImageData ManagedImageDataSet::level(int level) const {
	return images[level];
}

void ManagedImageDataSet::resize(int ws, int hs) {

	// once we have layers you can no longer resize the image vertically
	if ((layer != 0) && (hs != 1)) {
		FAULT("Can't resize the image vertically after a layer was already added!");
	}

	// actually resize all the image levels
	resizeImages(ws, hs);

	// update "base layer" height
	height *= hs;

}

void ManagedImageDataSet::blit(int ox, int oy, ImageData image, ImageScaling scaling) {
	std::vector<ImageData> temporaries;

	for (ImageData layer : images) {
		layer.blit(ox, oy, image);
		image = image.scaled(scaling);

		ox /= 2;
		oy /= 2;

		temporaries.push_back(image);
	}

	for (ImageData temporary : temporaries) {
		temporary.close();
	}
}

void ManagedImageDataSet::addLayer(ImageData image, ImageScaling scaling) {

	// offset to the next empty spot
	const int offset = layer * height;

	// width never changes when adding layers, the image only gets longer
	if (image.width() != level(0).width() || image.height() != height) {
		FAULT("Image dimensions don't match the sprite array!");
	}

	// check the "capacity", if too small, double the image in height
	if (level(0).height() <= offset + height) {
		resizeImages(1, 2);
	}

	blit(0, offset, image, scaling);
	layer ++;

}

size_t ManagedImageDataSet::size() const {
	size_t total = 0;

	for (ImageData image : images) {
		total += image.size();
	}

	return total;
}

ImageData ManagedImageDataSet::unified() const {

	int s = 0;
	int w = 0;
	int h = 0;

	for (ImageData image : images) {
		w += image.width();
		h = std::max(h, image.height());
	}

	ImageData collage = ImageData::allocate(w, h);

	for (ImageData image : images) {
		collage.blit(s, 0, image);
		s += image.width();
	}

	return collage;

}

void ManagedImageDataSet::save(const std::string& path) const {
	ImageData image = unified();
	image.save(path);
	image.close();
}

void ManagedImageDataSet::close() {
	for (ImageData image : images) {
		image.close();
	}

	images.clear();
}

MutableImage ManagedImageDataSet::upload(Allocator& allocator, CommandRecorder& recorder, VkFormat format) const {

	// dimensions of the base layer in the base level
	int layer_width = level(0).width();
	int layer_height = this->height;
	int layer_count = layers();

	// total image size (all layers and levels)
	size_t total = size();
	size_t offset = 0;

	// verify the given format again the first level (all levels are the same)
	if (getFormatInfo(format).size != (size_t) level(0).channels()) {
		FAULT("The specified image format doesn't match pixel size!");
	}

	Buffer staging = allocator.allocateBuffer(Memory::STAGED, total, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, "Image Staging");
	Image image = allocator.allocateImage(Memory::DEVICE, layer_width, layer_height, format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, layer_count, levels(), VK_SAMPLE_COUNT_1_BIT, "Untitled");

	MutableImage uploader {staging, image};
	int level = 0;

	// copy images level by level into staging buffer
	for (ImageData image : images) {
		uploader.write(offset, image, image.width(), std::max(1, (height >> level)), level);
		offset += image.size();
		level ++;
	}

	uploader.upload(recorder);
	return uploader;
}

/*
 * Image
 */

Image::Image(VkImage vk_image, VkFormat vk_format, Allocation allocation, uint16_t levels, uint16_t layers)
: levels(levels), layers(layers), vk_image(vk_image), vk_format(vk_format), allocation(allocation) {}

void Image::close() {
	allocation.closeImage(vk_image);
}

VkImage Image::getHandle() const {
	return vk_image;
}

VkFormat Image::getFormat() const {
	return vk_format;
}

int Image::getLayerCount() const {
	return layers;
}

int Image::getLevelCount() const {
	return levels;
}

/*
 * MutableImage
 */

MutableImage::MutableImage(Buffer& buffer, Image& image)
: staging(buffer), image(image) {
	map = buffer.getAllocation().map();
	writes.reserve(image.getLevelCount());
}

void MutableImage::close() {
	if (map) {
		image.close();
		staging.getAllocation().unmap();
		staging.close();
	}
}

void MutableImage::write(size_t offset, const ImageData& image, size_t width, size_t height, int level) {
	memcpy(static_cast<uint8_t*>(map) + offset, image.data(), image.size());
	writes.emplace_back(offset, width, height, level);
}

void MutableImage::upload(CommandRecorder& recorder) {
	staging.getAllocation().flushNonCoherent();
	recorder.transitionLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED);

	for (const auto & write : writes) {
		recorder.copyBufferToImage(image, staging, write.offset, write.width, write.height, image.getLayerCount(), write.level);
	}

	recorder.transitionLayout(image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	writes.clear();
}

Image MutableImage::discardStaging(TaskQueue& queue) {
	queue.enqueue([copy = staging] () mutable {
		copy.getAllocation().unmap();
		copy.close();
	});

	return image;
}

Image& MutableImage::getImage() {
	return image;
}

/*
 * ImageView
 */

ImageView ImageView::create(VkDevice device, const Image& image, VkImageViewType type, VkImageAspectFlags aspect) {

	VkImageViewCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	create_info.image = image.getHandle();

	create_info.viewType = type;
	create_info.format = image.getFormat();

	// just hardcode the swizzle
	create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	create_info.subresourceRange.aspectMask = aspect;
	create_info.subresourceRange.baseMipLevel = 0;
	create_info.subresourceRange.levelCount = image.getLevelCount();
	create_info.subresourceRange.baseArrayLayer = 0;
	create_info.subresourceRange.layerCount = image.getLayerCount();

	VkImageView view;

	if (vkCreateImageView(device, &create_info, nullptr, &view) != VK_SUCCESS) {
		FAULT("Failed to create image view!");
	}

	return ImageView {view};
}

ImageView::ImageView(VkImageView vk_view)
: vk_view(vk_view) {}

void ImageView::close(VkDevice vk_device) {
	if (!empty()) {
		vkDestroyImageView(vk_device, vk_view, nullptr);
	}
}

bool ImageView::empty() const {
	return vk_view == VK_NULL_HANDLE;
}

VkImageView ImageView::getHandle() const {
	return vk_view;
}