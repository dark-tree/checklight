#pragma once

#include "external.hpp"
#include "render/system.hpp"
#include "render/api/vertex.hpp"
#include "render/api/mesh.hpp"
#include "util/obj.hpp"

class Importer {

	public:

		static std::vector<std::shared_ptr<RenderMesh>> importObj(RenderSystem& system, const std::string& path) {
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

				for (auto& group : object.groups) {
					std::shared_ptr<RenderMesh> mesh = std::make_shared<RenderMesh>();
					mesh->uploadVertices(*commander, vertices);
					mesh->uploadIndices(*commander, group.indices);
					meshes.push_back(mesh);
				}
			}

			commander->complete();

			return meshes;
		}

};
