
#include "importer.hpp"

/*
 * Importer
 */

// TODO consider moving to render system, see comment in header
std::vector<std::shared_ptr<RenderMesh>> Importer::importObj(RenderSystem& system, const std::string& path) {
	std::vector<std::shared_ptr<RenderMesh>> meshes;

	std::map<std::string, std::shared_ptr<ObjMaterial>> materials;
	std::string mtlPath = "";

	try {
		mtlPath = ObjObject::getMtllib(path);

		if (!mtlPath.empty()) {
			materials = ObjMaterial::open(mtlPath);
		}

	} catch (const std::exception& e) {
		try {
			mtlPath = path.substr(0, path.find_last_of("/\\") + 1) + mtlPath;
			materials = ObjMaterial::open(mtlPath);

		} catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	auto scene = ObjObject::open(path, materials);

	auto commander = system.createTransientCommander();

	for (auto& object : scene) {
		std::vector<Vertex3D> vertices;

		for (auto& vertex : object.vertices) {
			float colorR = (vertex.normal.x + 1) / 2;
			float colorG = (vertex.normal.y + 1) / 2;
			float colorB = (vertex.normal.z + 1) / 2;
			vertices.push_back(Vertex3D(vertex.position.x, vertex.position.y, vertex.position.z, colorR, colorG, colorB));
		}

		for (int i = 0; i < object.groups.size(); i ++) {
			const auto& group = object.groups[i];
			std::shared_ptr<RenderMesh> mesh = system.createMesh();

			#if ENGINE_DEBUG
			std::string debug_name = "Mesh " + object.name + " #" + std::to_string(i) + " from " + path;
			mesh->setDebugName(debug_name.c_str());
			#endif

			mesh->uploadVertices(*commander, vertices);
			mesh->uploadIndices(*commander, group.indices);
			meshes.push_back(mesh);
		}
	}

	commander->complete();

	return meshes;
}