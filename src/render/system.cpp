
#include "system.hpp"

#include <memory>
#include "api/commander.hpp"
#include "api/mesh.hpp"
#include "api/model.hpp"

/*
 * RenderSystem
 */

std::unique_ptr<RenderSystem> RenderSystem::system {nullptr};

void RenderSystem::init(ApplicationParameters& parameters) {
	RenderSystem::system = std::make_unique<RenderSystem>(parameters);
}

RenderSystem::RenderSystem(ApplicationParameters& parameters)
: Renderer(parameters) {}

std::vector<std::shared_ptr<RenderModel>> RenderSystem::createRenderModels(std::vector<std::shared_ptr<RenderMesh>> meshes) {
	std::vector<std::shared_ptr<RenderModel>> models;

	for (auto& mesh : meshes) {
		AccelStructConfig config = AccelStructConfig::create(AccelStructConfig::BUILD, AccelStructConfig::BOTTOM);

		config.addTriangles(device, *mesh, true);
		config.setFlags(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);

		#if ENGINE_DEBUG
		config.setDebugName(mesh->name);
		#endif

		auto model = bakery.submit(device, allocator, config);
		model->setMesh(mesh);

		models.push_back(model);
	}

	return models;
}

std::unique_ptr<RenderCommander> RenderSystem::createTransientCommander() {
	CommandBuffer buffer = transient_pool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	CommandRecorder recorder = buffer.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	return std::make_unique<RenderCommander> (buffer, recorder, TaskQueue {});
}

std::shared_ptr<RenderMesh> RenderSystem::createMesh() {
	return std::make_shared<RenderMesh>();
}

void RenderSystem::setProjectionMatrix(float fov, float near_plane, float far_plane) {
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float) width() / (float) height(), near_plane, far_plane);
	projection[1][1] *= -1;

	getFrame().uniforms.projection = projection;
	getFrame().uniforms.projection_inv = glm::inverse(projection);

	getFrame().uniforms.near = near_plane;
	getFrame().uniforms.far = far_plane;
}

void RenderSystem::setViewMatrix(glm::vec3 eye, glm::vec3 direction) {
	getFrame().uniforms.view = glm::lookAt(eye, eye + direction, glm::vec3(0.0f, 1.0f, 0.0f));
	getFrame().uniforms.view_inv = glm::inverse(getFrame().uniforms.view);
}

std::shared_ptr<RenderObject> RenderSystem::createRenderObject() {
	return {instances->create()};
}

std::map<std::string, std::shared_ptr<ObjMaterial>> RenderSystem::importMaterials(const std::string& path) {
	std::map<std::string, std::shared_ptr<ObjMaterial>> materials;
	std::string mtl_path = "";

	try {
		mtl_path = ObjObject::getMtllib(path);

		if (!mtl_path.empty()) {
			materials = ObjMaterial::open(mtl_path);
		}
	}
	catch (const std::exception& e) {
		try {
			mtl_path = path.substr(0, path.find_last_of("/\\") + 1) + mtl_path;
			materials = ObjMaterial::open(mtl_path);
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

	return materials;
}

std::vector<std::shared_ptr<RenderModel>> RenderSystem::importObj(const std::string& path) {

	auto imported = importMaterials(path);

	auto open_texture = [&](std::string texture_path) -> TextureHandle {
		try {
			return materials.getTextureManager().createTexture(texture_path);
		}
		catch (const std::exception& e) {
			try {
				texture_path = path.substr(0, path.find_last_of("/\\") + 1) + texture_path;
				return materials.getTextureManager().createTexture(texture_path);
			}
			catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
				return TextureHandle {};
			}
		}
	};

	std::map<std::shared_ptr<ObjMaterial>, RenderMaterial> render_materials;

	for (auto& [name, material] : imported) {
		RenderMaterial& render_material = system->materials.createMaterial();

		render_material.albedo = glm::vec4(material->diffuse, material->alpha);
		render_material.emissive = material->emissive;
		render_material.specular = material->specular;
		render_material.shininess = material->shininess;

		if (!material->diffuseMap.empty()) {
			render_material.albedo_texture = open_texture(material->diffuseMap);
		}

		render_materials[material] = render_material;
	}

	auto scene = ObjObject::open(path, imported);

	std::vector<std::shared_ptr<RenderMesh>> meshes;

	// FIXME you should not create the commander multiple times
	//       create one and reuse, this is very slow
	auto commander = system->createTransientCommander();

	system->materials.flush(system->allocator, commander->getRecorder(), commander->getTaskQueue(), system->device);

	for (auto& object : scene) {
		std::shared_ptr<RenderMesh> mesh = system->createMesh();

		#if ENGINE_DEBUG
		std::string debug_name = "Mesh " + object.name + " from " + path;
		mesh->setDebugName(debug_name.c_str());
		#endif

		std::vector<Vertex3D> vertices;

		for (auto& vertex : object.vertices) {
			float r = (vertex.normal.x + 1) / 2;
			float g = (vertex.normal.y + 1) / 2;
			float b = (vertex.normal.z + 1) / 2;
			vertices.emplace_back(vertex.position.x, vertex.position.y, vertex.position.z, r * 255, g * 255, b * 255, 255, vertex.uv.x, 1.0 - vertex.uv.y, 0);
		}	

		std::vector<uint32_t> indices;

		for (size_t i = 0; i < object.groups.size(); i++) {
			const auto& group = object.groups[i];
			indices.insert(indices.end(), group.indices.begin(), group.indices.end());

			// Assign material to vertices
			RenderMaterial& material = render_materials[group.material];

			for (uint32_t vertex_index : group.indices) {
				vertices[vertex_index].material_index = material.index;
			}
		}

		mesh->uploadVertices(*commander, vertices);
		mesh->uploadIndices(*commander, indices);

		meshes.push_back(mesh);
	}

	auto models = createRenderModels(meshes);
	rebuildBottomLevel(commander->getRecorder());

	commander->complete();

	return models;
}

void RenderSystem::closeModel(std::shared_ptr<RenderModel> model) {
	model->close(system->device);
}

RenderParameters& RenderSystem::getParameters() {
	return this->parameters;
}

void RenderSystem::draw() {

	SceneUniform& scene = getFrame().uniforms;

	scene.time = glfwGetTime();

	parameters.updateSceneUniform(scene);

	Renderer::draw();

	scene.prev_projection = scene.projection;
	scene.prev_projection_inv = scene.projection_inv;

	scene.prev_view = scene.view;
	scene.prev_view_inv = scene.view_inv;
}
