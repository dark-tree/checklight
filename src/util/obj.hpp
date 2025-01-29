#pragma once

#include "external.hpp"

struct ObjVertex {
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 normal = glm::vec3(0.0f);
	glm::vec2 uv = glm::vec2(0.0f);
};

struct ObjMaterial {
	std::string name;

	glm::vec3 ambient = glm::vec3(0.0f);
	glm::vec3 diffuse = glm::vec3(0.0f);
	glm::vec3 specular = glm::vec3(0.0f);
	float shininess = 0.0f;
	float alpha = 1.0f;

	std::string ambientMap = "";
	std::string diffuseMap = "";
	std::string specularMap = "";
	std::string shininessMap = "";
	std::string normalMap = "";
	std::string alphaMap = "";

	int illuminationModel = 0;

	static std::map<std::string, std::shared_ptr<ObjMaterial>> open(std::string path) {
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
};

struct ObjGroup {
	std::string name = "";
	std::vector<int> indices;
	std::shared_ptr<ObjMaterial> material = nullptr;
};

struct ObjObject {

	private:

		// offset of the first vertex of the object in the global vertices array
		int vertexOffset = 0;

		// number of vertices that have been added to the global vertices array due to duplicates
		// duplicates are vertices that have the same position but different texture or normal
		int extraVertexCount = 0;

		int extraVertexOffset = 0;

	public:

		std::string name;
		std::vector<ObjVertex> vertices;
		std::vector<ObjGroup> groups;

		static std::string getMtllib(std::string path) {
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

		static std::vector<ObjObject> open(std::string path, const std::map<std::string, std::shared_ptr<ObjMaterial>>& materials) {
			std::vector<ObjObject> objects;

			std::ifstream file(path);
			if (!file.is_open()) {
				throw std::runtime_error("Failed to open file: " + path);
			}

			std::vector<glm::vec3> globalNormals;
			std::vector<glm::vec2> globalUVs;

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
					globalNormals.push_back(normal);
				}
				else if (token == "vt") {
					glm::vec2 uv;
					stream >> uv.x >> uv.y;
					globalUVs.push_back(uv);
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

					int vertexCount = 0;

					// vertexString is a string of the form "vertexIndex/textureIndex/normalIndex"
					std::string vertexString;

					while (stream >> vertexString) {
						vertexCount++;
						int vertexIndex = std::stoi(vertexString);

						// vertexIndex can be negative, which means it is relative to the end of the list
						if (vertexIndex < 0) {
							vertexIndex = (objects.back().vertices.size() + objects.back().extraVertexOffset - objects.back().extraVertexCount) + vertexIndex + 1;
						}

						int uvIndex = 0;
						int normalIndex = 0;

						int separatorIndex = vertexString.find('/');
						if (separatorIndex != std::string::npos) {
							vertexString = vertexString.substr(separatorIndex + 1);
							if (vertexString[0] != '/') {
								uvIndex = std::stoi(vertexString);
								if (uvIndex < 0) {
									uvIndex = globalUVs.size() + uvIndex + 1;
								}
							}

							separatorIndex = vertexString.find('/');
							if (separatorIndex != std::string::npos) {
								normalIndex = std::stoi(vertexString.substr(separatorIndex + 1));
								if (normalIndex < 0) {
									normalIndex = globalNormals.size() + normalIndex + 1;
								}
							}
						}

						int localVertexIndex = vertexIndex - 1 - objects.back().vertexOffset + objects.back().extraVertexOffset;

						ObjVertex& vert = objects.back().vertices[localVertexIndex];
						ObjVertex newVertex = vert;

						if (uvIndex > 0) {
							newVertex.uv = globalUVs[uvIndex - 1];
						}
						if (normalIndex > 0) {
							newVertex.normal = globalNormals[normalIndex - 1];
						}

						if (vert.normal == glm::vec3(0.0f) && vert.uv == glm::vec2(0.0f)) {
							// this vertex has not been processed yet
							vert = newVertex;
							objects.back().groups.back().indices.push_back(localVertexIndex);
						}
						else {
							if (vert.uv == newVertex.uv && vert.normal == newVertex.normal) {
								// this vertex has been processed and is the same as the new one
								objects.back().groups.back().indices.push_back(localVertexIndex);
							}
							else {
								while (true) {
									// this vertex has been processed and is different from the new one
									// check if there is a vertex with the same position and uv
									int duplicateVertexIndex = duplicates[localVertexIndex];
									if (duplicateVertexIndex == -1) {
										// there is no vertex with the same position and texture
										// create a new vertex
										objects.back().vertices.push_back(newVertex);
										duplicates.push_back(-1);
										duplicates[localVertexIndex] = objects.back().vertices.size() - 1;
										objects.back().groups.back().indices.push_back(objects.back().vertices.size() - 1);
										objects.back().extraVertexCount++;
										break;
									}
									else {
										if (objects.back().vertices[duplicateVertexIndex].uv == newVertex.uv && objects.back().vertices[duplicateVertexIndex].normal == newVertex.normal) {
											// there is a vertex with the same position, uv and normal, use it
											objects.back().groups.back().indices.push_back(duplicateVertexIndex);
											break;
										}
										else {
											localVertexIndex = duplicateVertexIndex;
										}
									}
								}
							}
						}
					}
					// TODO: Handle polygons with more than 3 vertices
				}

			}

			return objects;
		}
};
