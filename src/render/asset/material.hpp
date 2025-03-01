#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/texture.hpp"
#include "render/vulkan/command/recorder.hpp"
#include "render/vulkan/descriptor/descriptor.hpp"
#include "render/api/reusable.hpp"

struct TextureHandle {
	uint32_t index;
};

class TextureManager {

	public:

		TextureManager();

		/**
		* Frees all resources
		*/
		void close(const LogicalDevice& device);

		/**
		* Creates a texture from a file. The texture is not uploaded to the GPU until `upload()` is called
		*/
		TextureHandle createTexture(std::string path);

		/**
		* Uploads all pending textures to the GPU
		*/
		void flush(Allocator& allocator, CommandRecorder& recorder, TaskQueue& queue, LogicalDevice& device);

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

struct RenderMaterial {
	uint32_t index;
	
	glm::vec4 albedo = glm::vec4(1.0f);
	glm::vec3 specular = glm::vec3(0.0f);
	float shininess = 0.0f;

	TextureHandle albedo_texture;
};

class MaterialManager {

	public:

		MaterialManager();

		/**
		* Frees all resources, including the TextureManager
		*/
		void close(const LogicalDevice& device);

		/**
		* Creates a new material. The material is not uploaded to the GPU until `flush()` is called
		*/
		RenderMaterial& createMaterial();

		/**
		* Uploads all pending materials to the GPU and flushes the TextureManager
		*/
		void flush(Allocator& allocator, CommandRecorder& recorder, TaskQueue& queue, LogicalDevice& device);

		/**
		* Returns the material buffer
		*/
		const ReusableBuffer& getMaterialBuffer();

		/**
		* Returns the TextureManager
		*/
		TextureManager& getTextureManager();

	private:

		size_t capacity;
		TextureManager texture_manager;
		std::vector<RenderMaterial> materials;
		ReusableBuffer material_buffer;

};
