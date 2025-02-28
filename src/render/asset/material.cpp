
#include "material.hpp"

/**
 * TextureManager
 */

TextureManager::TextureManager() {

	ImageData image_data = ImageData::allocate(2, 2, 4);
	image_data.clear({255, 255, 255, 255});

	TextureDelegate tdelegate = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R8G8B8A8_SRGB)
		.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
		.createDelegate();

	pending_creation.push_back({ image_data, tdelegate });

	initialized = false;
	dirty = true;
}

void TextureManager::close(const LogicalDevice& device) {

	for (auto& pending : pending_creation) {
		pending.first.close();
	}

	for (auto& texture : textures) {
		texture.closeImageViewSampler(device.getHandle());
	}
}

TextureHandle TextureManager::createTexture(std::string path) {
	
	if (textures.size() + pending_creation.size() >= MAX_TEXTURES) {
		throw std::runtime_error {"Maximum number of textures reached!"};
	}

	TextureHandle handle;
	handle.index = textures.size() + pending_creation.size();

	ImageData image_data = ImageData::loadFromFile(path);

	TextureDelegate tdelegate = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R8G8B8A8_SRGB)
		.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
		.setDebugName(path + " Handle: " + std::to_string(handle.index))
		.createDelegate();
	
	pending_creation.push_back({image_data, tdelegate});

	return handle;
}

void TextureManager::flush(Allocator& allocator, CommandRecorder& recorder, TaskQueue& queue, LogicalDevice& device) {

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

/**
 * MaterialManager
 */

MaterialManager::MaterialManager() 
: texture_manager(), material_buffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
	material_buffer.setDebugName("Material Buffer");

	// Create a default material
	createMaterial();
}

void MaterialManager::close(const LogicalDevice& device) {
	material_buffer.close();
	texture_manager.close(device);
}

RenderMaterial& MaterialManager::createMaterial() {

	RenderMaterial material {};
	material.index = materials.size();

	materials.push_back(material);

	return materials.back();
}

void MaterialManager::flush(Allocator& allocator, CommandRecorder& recorder, TaskQueue& queue, LogicalDevice& device) {
	
	material_buffer.resize(materials.size(), sizeof(RenderMaterial));
	material_buffer.writeToStaging(materials.data(), materials.size(), sizeof(RenderMaterial), 0);

	material_buffer.flushStaging(recorder);

	texture_manager.flush(allocator, recorder, queue, device);
}

const ReusableBuffer& MaterialManager::getMaterialBuffer() {
	return material_buffer;
}

TextureManager& MaterialManager::getTextureManager() {
	return texture_manager;
}
