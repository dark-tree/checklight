
#include "material.hpp"

TextureManager::TextureManager() {

	ImageData image_data = ImageData::allocate(2, 2, 4);

	TextureDelegate tdelegate = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R8G8B8A8_SRGB)
		.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
		.createDelegate();

	pending_creation.push_back({ image_data, tdelegate });

	initialized = false;
	dirty = true;
}

TextureManager::~TextureManager() {
	for (auto& pending : pending_creation) {
		pending.first.close();
	}

	// TODO: free textures; Need a way to access VkDevice.
}

TextureHandle TextureManager::createTexture(std::string path) {
	
	TextureHandle handle;
	handle.index = textures.size() + pending_creation.size();

	ImageData image_data = ImageData::loadFromFile(path);

	TextureDelegate tdelegate = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R8G8B8A8_SRGB)
		.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
		.createDelegate();
	
	pending_creation.push_back({image_data, tdelegate});

	return handle;
}

void TextureManager::upload(Allocator& allocator, CommandRecorder& recorder, TaskQueue& queue, LogicalDevice& device) {

	for (auto& [image_data, tdelegate] : pending_creation) {
		auto image = image_data.upload(allocator, recorder, queue, VK_FORMAT_R8G8B8A8_SRGB, false);
		Texture texture = tdelegate.buildTexture(device, image);
		textures.push_back(texture);
		image_data.close();
	}

	pending_creation.clear();

	dirty = true;
}

void TextureManager::updateDescriptorSet(LogicalDevice& device, DescriptorSet& set, uint32_t binding) {

	if (dirty) {
		if (initialized) {
			// Update only the descriptors that are not empty
			set.samplerArray(binding, textures, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
		else {
			// Update all descriptors
			std::vector<Texture> all_textures(MAX_TEXTURES);

			for (size_t i = 0; i < MAX_TEXTURES; i++) {
				if (i < textures.size()) {
					all_textures[i] = textures[i];
				}
				else {
					all_textures[i] = textures[0];
				}
			}

			set.samplerArray(binding, all_textures, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			initialized = true;
		}
		dirty = false;
	}
}

Material& MaterialManager::createMaterial() {
	Material material;
	material.index = materials.size();
	materials.push_back(material);
	return materials.back();
}

const std::vector<Material>& MaterialManager::getMaterials() const {
	return materials;
}

TextureManager& MaterialManager::getTextureManager() {
	return texture_manager;
}
