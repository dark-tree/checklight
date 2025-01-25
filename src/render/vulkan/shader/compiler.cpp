
#include "compiler.hpp"
#include "shader.hpp"
#include "render/vulkan/setup/device.hpp"

const Kind Kind::VERTEX = {shaderc_vertex_shader, VK_SHADER_STAGE_VERTEX_BIT};
const Kind Kind::FRAGMENT = {shaderc_fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT};
const Kind Kind::COMPUTE = {shaderc_compute_shader, VK_SHADER_STAGE_COMPUTE_BIT};
const Kind Kind::GEOMETRY = {shaderc_geometry_shader, VK_SHADER_STAGE_GEOMETRY_BIT};
const Kind Kind::TESSELLATION_CONTROL = {shaderc_tess_control_shader, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT};
const Kind Kind::TESSELLATION_EVALUATION = {shaderc_tess_evaluation_shader, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT};

// from VK_KHR_ray_tracing_pipeline
const Kind Kind::RAYGEN {shaderc_raygen_shader, VK_SHADER_STAGE_RAYGEN_BIT_KHR};
const Kind Kind::ANYHIT {shaderc_anyhit_shader, VK_SHADER_STAGE_ANY_HIT_BIT_KHR};
const Kind Kind::CLOSEST {shaderc_closesthit_shader, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR};
const Kind Kind::MISS {shaderc_miss_shader, VK_SHADER_STAGE_MISS_BIT_KHR};
const Kind Kind::INTERSECTION {shaderc_intersection_shader, VK_SHADER_STAGE_INTERSECTION_BIT_KHR};
const Kind Kind::CALLABLE {shaderc_callable_shader, VK_SHADER_STAGE_CALLABLE_BIT_KHR};

/*
 * Compiler::ResultBuilder
 */

Compiler::ResultBuilder::ResultBuilder(Kind kind, const char* name)
: kind(kind), name(name) {}

Shader Compiler::ResultBuilder::build(LogicalDevice& device, bool successful) {
	if (!successful) {
		printf("ERROR: %s\n", errors.c_str());
		throw std::runtime_error {"Unable to create shader module!"};
	}

	return {device, output.data(), (uint32_t) (output.size() * sizeof(uint32_t)), kind.vulkan};
}

/*
 * Compiler
 */

Compiler::Compiler() {
	#if defined(NDEBUG)
	options.SetOptimizationLevel(shaderc_optimization_level_performance);
	#else
	options.SetGenerateDebugInfo();
	#endif
}

void Compiler::close() {
	for (Shader& shader : shaders) shader.close();
}

Shader Compiler::compileFile(LogicalDevice& device, const std::string& identifier, Kind kind) {
	std::ifstream stream {identifier};

	if (stream.fail()) {
		throw std::runtime_error {"Failed to open shader file '" + identifier + "'"};
	}

	std::stringstream buffer;
	buffer << stream.rdbuf();

	return compileString(device, identifier, buffer.str(), kind);
}

Shader Compiler::compileString(LogicalDevice& device, const std::string& unit, const std::string& source, Kind kind) {
	ResultBuilder builder {kind, unit.c_str()};

	// preprocessor
	const auto pre_result = compiler.PreprocessGlsl(source, kind.shaderc, unit.c_str(), options);
	builder.copyMessages(pre_result);

	if (pre_result.GetCompilationStatus() != shaderc_compilation_status_success) {
		return builder.build(device, false);
	}

	// the code after the preprocessor stage
	std::string preprocessed {pre_result.begin(), pre_result.end()};

	// compiler
	const auto compile_result = compiler.CompileGlslToSpv(preprocessed, kind.shaderc, unit.c_str(), options);
	builder.copyMessages(compile_result);

	if (compile_result.GetCompilationStatus() != shaderc_compilation_status_success) {
		return builder.build(device, false);
	}

	builder.copyOutput(compile_result);
	Shader shader = builder.build(device, true);
	shaders.emplace_back(shader);
	return shader;
}