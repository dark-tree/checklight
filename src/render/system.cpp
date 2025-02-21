
#include "system.hpp"
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
		config.setDebugName("<model>"); // TODO

		auto model = bakery.submit(device, allocator, config);
		model->setMesh(mesh);

		models.push_back(model);
	}

	return models;
}

std::unique_ptr<RenderCommander> RenderSystem::createTransientCommander() {

	CommandBuffer buffer = transient_pool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	CommandRecorder recorder = buffer.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// TODO why tf does make_unique not work here?
	return std::unique_ptr<RenderCommander> {new RenderCommander {RenderCommander::TRANSIENT, buffer, recorder, {}}};

}

std::shared_ptr<RenderMesh> RenderSystem::createMesh() {
	return std::make_shared<RenderMesh>();
}

void RenderSystem::setProjectionMatrix(float fov, float near_plane, float far_plane) {
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float) width() / (float) height(), near_plane, far_plane);
	projection[1][1] *= -1;

	getFrame().uniforms.projection = projection;
}

void RenderSystem::setViewMatrix(glm::vec3 eye, glm::vec3 direction) {
	getFrame().uniforms.view = glm::lookAt(eye, eye + direction, glm::vec3(0.0f, 1.0f, 0.0f));
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

std::vector<std::shared_ptr<RenderMesh>> RenderSystem::importObj(const std::string& path) {

	auto materials = importMaterials(path);

	auto open_texture = [&](std::string texture_path) -> TextureHandle {
		try {
			return system->materials.getTextureManager().createTexture(texture_path);
		}
		catch (const std::exception& e) {
			try {
				texture_path = path.substr(0, path.find_last_of("/\\") + 1) + texture_path;
				return system->materials.getTextureManager().createTexture(texture_path);
			}
			catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
				return TextureHandle {};
			}
		}
	};

	std::map<std::shared_ptr<ObjMaterial>, Material> render_materials;

	for (auto& [name, material] : materials) {
		Material& render_material = system->materials.createMaterial();

		if (!material->diffuseMap.empty()) {
			render_material.albedo_texture = open_texture(material->diffuseMap);
		}

		render_materials[material] = render_material;
	}

	auto scene = ObjObject::open(path, materials);

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
			Material& material = render_materials[group.material];

			for (uint32_t vertex_index : group.indices) {
				vertices[vertex_index].material_index = material.index;
			}
		}

		mesh->uploadVertices(*commander, vertices);
		mesh->uploadIndices(*commander, indices);

		meshes.push_back(mesh);
	}

	commander->complete();

	return meshes;
}
