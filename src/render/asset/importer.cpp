
#include "importer.hpp"

/*
 * Importer
 */

// TODO consider moving to render system, see comment in header
std::vector<std::shared_ptr<RenderMesh>> Importer::importObj(RenderSystem& system, const std::string& path) {
	std::vector<std::shared_ptr<RenderMesh>> meshes;

	std::map<std::string, std::shared_ptr<ObjMaterial>> materials;
	std::string mtl_path = "";

	try {
		mtl_path = ObjObject::getMtllib(path);

		if (!mtl_path.empty()) {
			materials = ObjMaterial::open(mtl_path);
		}

	} catch (const std::exception& e) {
		try {
			mtl_path = path.substr(0, path.find_last_of("/\\") + 1) + mtl_path;
			materials = ObjMaterial::open(mtl_path);

		} catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	auto scene = ObjObject::open(path, materials);

	// FIXME you should not create the commander multiple times
	//       create one and reuse, this is very slow
	auto commander = system.createTransientCommander();

	for (auto& object : scene) {
		std::shared_ptr<RenderMesh> mesh = system.createMesh();

		#if ENGINE_DEBUG
		std::string debug_name = "Mesh " + object.name + " from " + path;
		mesh->setDebugName(debug_name.c_str());
		#endif

		{
			std::vector<Vertex3D> vertices;

			for (auto& vertex : object.vertices) {
				float r = (vertex.normal.x + 1) / 2;
				float g = (vertex.normal.y + 1) / 2;
				float b = (vertex.normal.z + 1) / 2;
				vertices.emplace_back(vertex.position.x, vertex.position.y, vertex.position.z, r * 255, g * 255, b * 255, 255, 0, 0);
			}

			mesh->uploadVertices(*commander, vertices);
		}

		{
			std::vector<uint32_t> indices;

			for (size_t i = 0; i < object.groups.size(); i ++) {
				const auto& group = object.groups[i];
				indices.insert(indices.end(), group.indices.begin(), group.indices.end());
			}

			mesh->uploadIndices(*commander, indices);
		}
		
		meshes.push_back(mesh);
	}

	commander->complete();

	return meshes;
}