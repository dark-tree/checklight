#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/texture.hpp"
#include "render/vulkan/command/recorder.hpp"
#include "render/vulkan/descriptor/descriptor.hpp"

struct TextureHandle {
	uint32_t index;
};

class TextureManager {

	public:

		TextureManager();
		~TextureManager();

		/**
		* Creates a texture from a file. The texture is not uploaded to the GPU until `upload()` is called
		*/
		TextureHandle createTexture(std::string path);

		/**
		* Uploads all pending textures to the GPU
		*/
		void upload(Allocator& allocator, CommandRecorder& recorder, TaskQueue& queue, LogicalDevice& device);

		/**
		* Updates the descriptor set with the textures
		*/
		void updateDescriptorSet(LogicalDevice& device, DescriptorSet& set, uint32_t binding);

		/**
		* Maximum number of textures that can be created and stored in a single TextureManager instance
		*/
		static const uint32_t MAX_TEXTURES = 100;

	private:

		bool initialized = false;
		bool dirty = false;
		std::vector<std::pair<ImageData, TextureDelegate>> pending_creation;
		std::vector<Texture> textures;

};

struct Material {
	uint32_t index;
	TextureHandle albedo_texture;
};

// TODO
class MaterialManager {

	public:

		Material& createMaterial();

		const std::vector<Material>& getMaterials() const;

		TextureManager& getTextureManager();

	private:

		TextureManager texture_manager;
		std::vector<Material> materials;
		//ReusableBuffer material_buffer;

};
