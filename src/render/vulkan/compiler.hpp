#pragma once

#include "external.hpp"

class LogicalDevice;
class Shader;

/**
 * Java-style enum representing the shader type,
 * combines the vulkan and ShaderC shader type enums
 */
class Kind {

	public:

		static const Kind VERTEX;
		static const Kind FRAGMENT;
		static const Kind COMPUTE;
		static const Kind GEOMETRY;
		static const Kind TESSELLATION_CONTROL;
		static const Kind TESSELLATION_EVALUATION;
		static const Kind RAYGEN;
		static const Kind ANYHIT;
		static const Kind CLOSEST;
		static const Kind MISS;
		static const Kind INTERSECTION;
		static const Kind CALLABLE;
		static const Kind TASK;
		static const Kind MESH;

	public:

		const shaderc_shader_kind shaderc;
		const VkShaderStageFlagBits vulkan;

		inline constexpr Kind(shaderc_shader_kind shaderc, VkShaderStageFlagBits stage)
		: shaderc(shaderc), vulkan(stage) {}

};

class Compiler {

	private:

		const shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		class ResultBuilder {

			private:

				Kind kind;
				const char* name;
				std::string errors;
				std::vector<uint32_t> output;

			public:

				ResultBuilder(Kind kind, const char* name);
				Shader build(LogicalDevice& device, bool successful);

			public:

				template<typename T>
				void copyMessages(const shaderc::CompilationResult<T>& result) {
					errors += result.GetErrorMessage();
				}

				template<typename T>
				void copyOutput(const shaderc::CompilationResult<T>& result) {
					std::copy(result.begin(), result.end(), std::back_inserter(output));
				}

		};

	public:

		Compiler();

		Shader compileFile(LogicalDevice& device, const std::string& identifier, Kind kind);
		Shader compileString(LogicalDevice& device, const std::string& unit, const std::string& source, Kind kind);

};
