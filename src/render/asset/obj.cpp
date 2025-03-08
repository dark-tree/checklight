
#include "obj.hpp"

/*
 * ObjMaterial
 */

std::map<std::string, std::shared_ptr<ObjMaterial>> ObjMaterial::open(std::string path) {
	std::map<std::string, std::shared_ptr<ObjMaterial>> materials;

	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + path);
	}

	std::shared_ptr<ObjMaterial> material = std::make_shared<ObjMaterial>();

	std::string line;
	while (std::getline(file, line)) {

		std::istringstream stream(line);
		std::string token;
		stream >> token;

		if (token == "newmtl") {
			material = std::make_shared<ObjMaterial>();
			stream >> material->name;
			materials.insert({ material->name, material });
		}
		else if (token == "Ka") {
			glm::vec3 ambient;
			stream >> ambient.r >> ambient.g >> ambient.b;
			material->ambient = ambient;
		}
		else if (token == "Ke") {
			glm::vec3 emissive;
			stream >> emissive.r >> emissive.g >> emissive.b;
			material->emissive = emissive;
		}
		else if (token == "Kd") {
			glm::vec3 diffuse;
			stream >> diffuse.r >> diffuse.g >> diffuse.b;
			material->diffuse = diffuse;
		}
		else if (token == "Ks") {
			glm::vec3 specular;
			stream >> specular.r >> specular.g >> specular.b;
			material->specular = specular;
		}
		else if (token == "Ns") {
			float shininess;
			stream >> shininess;
			material->shininess = shininess;
		}
		else if (token == "d") {
			float alpha;
			stream >> alpha;
			material->alpha = alpha;
		}
		else if (token == "Tr") {
			float alpha;
			stream >> alpha;
			material->alpha = 1.0f - alpha;
		}
		else if (token == "map_Ka") {
			std::string ambientMap;
			stream >> ambientMap;
			material->ambientMap = ambientMap;
		}
		else if (token == "map_Kd") {
			std::string diffuseMap;
			stream >> diffuseMap;
			material->diffuseMap = diffuseMap;
		}
		else if (token == "map_Ks" || token == "map_refl" || token == "map_Pm") {
			// TODO: refl and Pm are metalness maps, not specular maps
			std::string specularMap;
			stream >> specularMap;
			material->specularMap = specularMap;
		}
		else if (token == "map_Ns" || token == "map_Pr") {
			// TODO: Pr is roughness maps, not shininess maps
			std::string shininessMap;
			stream >> shininessMap;
			material->shininessMap = shininessMap;
		}
		else if (token == "map_Bump" || token == "map_bump") {
			std::string normalMap;
			stream >> normalMap;
			if (!normalMap.empty() && normalMap[0] == '-') {
				stream >> normalMap;
			}
			material->normalMap = normalMap;
		}
		else if (token == "map_d") {
			std::string alphaMap;
			stream >> alphaMap;
			material->alphaMap = alphaMap;
		}
		else if (token == "illum") {
			int illuminationModel;
			stream >> illuminationModel;
			material->illuminationModel = illuminationModel;
		}
	}

	return materials;
}

/*
 * ObjObject
 */

std::string ObjObject::getMtllib(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + path);
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream stream(line);
		std::string token;
		stream >> token;

		if (token == "mtllib") {
			std::string mtllib;
			stream >> mtllib;
			return mtllib;
		}
	}

	return "";
}

std::vector<ObjObject> ObjObject::open(std::string path, const std::map<std::string, std::shared_ptr<ObjMaterial>>& materials) {
	std::vector<ObjObject> objects;

	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + path);
	}

	std::vector<glm::vec3> global_normals;
	std::vector<glm::vec2> global_uvs;

	// linked list of duplicates
	std::vector<int> duplicates;

	std::string line;
	while (std::getline(file, line)) {

		std::istringstream stream(line);
		std::string token;
		stream >> token;

		if (token == "o") {
			ObjObject object;
			stream >> object.name;

			if (!objects.empty()) {
				duplicates.clear();
				object.vertexOffset = objects.back().vertices.size() - objects.back().extraVertexCount + objects.back().vertexOffset;
			}

			objects.push_back(object);
		}
		else if (token == "v") {
			ObjVertex vertex;
			stream >> vertex.position.x >> vertex.position.y >> vertex.position.z;

			objects.back().vertices.push_back(vertex);
			duplicates.push_back(-1);

			objects.back().extraVertexOffset = objects.back().extraVertexCount;
		}
		else if (token == "vn") {
			glm::vec3 normal;
			stream >> normal.x >> normal.y >> normal.z;
			global_normals.push_back(normal);
		}
		else if (token == "vt") {
			glm::vec2 uv;
			stream >> uv.x >> uv.y;
			global_uvs.push_back(uv);
		}
		else if (token == "g") {
			ObjGroup group;
			stream >> group.name;
			objects.back().groups.push_back(group);
		}
		else if (token == "usemtl") {
			if (objects.back().groups.empty()) {
				objects.back().groups.push_back(ObjGroup{});
			}

			std::string name;
			stream >> name;

			auto material = materials.find(name);

			if (material != materials.end()) {
				objects.back().groups.back().material = material->second;
			}
		}
		else if (token == "f") {
			if (objects.back().groups.empty()) {
				objects.back().groups.push_back(ObjGroup{});
			}

			// vertexString is a string of the form "vertexIndex/textureIndex/normalIndex"
			std::string vertex_string;
			int vertex_count = 0;

			while (stream >> vertex_string) {
				vertex_count ++;
				int vertex_index = std::stoi(vertex_string);

				// vertexIndex can be negative, which means it is relative to the end of the list
				if (vertex_index < 0) {
					vertex_index = (objects.back().vertices.size() + objects.back().extraVertexOffset - objects.back().extraVertexCount) + vertex_index + 1;
				}

				int uvIndex = 0;
				int normal_index = 0;

				auto separator_index = vertex_string.find('/');
				if (separator_index != std::string::npos) {
					vertex_string = vertex_string.substr(separator_index + 1);
					if (vertex_string[0] != '/') {
						uvIndex = std::stoi(vertex_string);
						if (uvIndex < 0) {
							uvIndex = global_uvs.size() + uvIndex + 1;
						}
					}

					separator_index = vertex_string.find('/');
					if (separator_index != std::string::npos) {
						normal_index = std::stoi(vertex_string.substr(separator_index + 1));
						if (normal_index < 0) {
							normal_index = global_normals.size() + normal_index + 1;
						}
					}
				}

				int local_vertex_index = vertex_index - 1 - objects.back().vertexOffset + objects.back().extraVertexOffset;

				ObjVertex& vertex = objects.back().vertices[local_vertex_index];
				ObjVertex new_vertex = vertex;

				if (uvIndex > 0) {
					new_vertex.uv = global_uvs[uvIndex - 1];
				}
				if (normal_index > 0) {
					new_vertex.normal = global_normals[normal_index - 1];
				}

				if (vertex.normal == glm::vec3(0.0f) && vertex.uv == glm::vec2(0.0f)) {
					// this vertex has not been processed yet
					vertex = new_vertex;
					objects.back().groups.back().indices.push_back(local_vertex_index);
				}
				else {
					if (vertex.uv == new_vertex.uv && vertex.normal == new_vertex.normal) {
						// this vertex has been processed and is the same as the new one
						objects.back().groups.back().indices.push_back(local_vertex_index);
					}
					else {
						while (true) {
							// this vertex has been processed and is different from the new one
							// check if there is a vertex with the same position and uv
							int duplicate_vertex_index = duplicates[local_vertex_index];
							if (duplicate_vertex_index == -1) {
								// there is no vertex with the same position and texture
								// create a new vertex
								objects.back().vertices.push_back(new_vertex);
								duplicates.push_back(-1);
								duplicates[local_vertex_index] = objects.back().vertices.size() - 1;
								objects.back().groups.back().indices.push_back(objects.back().vertices.size() - 1);
								objects.back().extraVertexCount++;
								break;
							}
							else {
								if (objects.back().vertices[duplicate_vertex_index].uv == new_vertex.uv && objects.back().vertices[duplicate_vertex_index].normal == new_vertex.normal) {
									// there is a vertex with the same position, uv and normal, use it
									objects.back().groups.back().indices.push_back(duplicate_vertex_index);
									break;
								}
								else {
									local_vertex_index = duplicate_vertex_index;
								}
							}
						}
					}
				}
			}

			if (vertex_count > 3) {
				throw std::runtime_error {"Unsupported polygon!"};
			}
		}

	}

	return objects;
}
