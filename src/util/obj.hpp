#pragma once

#include "external.hpp"

namespace obj {

	using index = uint32_t;

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};

	struct Material {
		std::string name;
	};

	struct Group {
		std::string name = "";
		std::vector<index> indices;
		std::shared_ptr<Material> material;
	};

	struct Object {

		private:

			std::vector<glm::vec3> tempNormals;
			std::vector<glm::vec2> tempUVs;

			// offset of the first vertex of the object in the global vertices array
			int vertexOffset = 0;

			// number of vertices that have been added to the global vertices array due to duplicates
			// duplicates are vertices that have the same position but different texture or normal
			int extraVertexCount = 0;

			// linked list of duplicates
			std::vector<int> duplicates;

		public:

			std::string name;
			std::vector<Vertex> vertices;
			std::vector<Group> groups;

			friend class ObjScene;

	};

	class ObjScene {

		private:

			std::vector<Object> objects;

		public:

			ObjScene() = default;

			ObjScene(std::string path) {
				open(path);
			}

			void open(std::string path) {
				std::ifstream file(path);
				if (!file.is_open()) {
					throw std::runtime_error("Failed to open file: " + path);
				}

				std::string line;
				while (std::getline(file, line)) {

					std::istringstream stream(line);
					std::string token;
					stream >> token;

					if (token == "o") {
						Object object;
						stream >> object.name;

						if (!objects.empty()) {
							objects.back().tempNormals.clear();
							objects.back().tempUVs.clear();
							objects.back().duplicates.clear();
							object.vertexOffset = objects.back().vertices.size() - objects.back().extraVertexCount;
						}

						objects.push_back(object);
					}
					else if (token == "v") {
						Vertex vertex;
						stream >> vertex.position.x >> vertex.position.y >> vertex.position.z;

						objects.back().vertices.push_back(vertex);
						objects.back().duplicates.push_back(-1);
					}
					else if (token == "vn") {
						glm::vec3 normal;
						stream >> normal.x >> normal.y >> normal.z;
						objects.back().tempNormals.push_back(normal);
					}
					else if (token == "vt") {
						glm::vec2 uv;
						stream >> uv.x >> uv.y;
						objects.back().tempUVs.push_back(uv);
					}
					else if (token == "g") {
						Group group;
						stream >> group.name;
						objects.back().groups.push_back(group);
					}
					else if (token == "usemtl") {
						if (objects.back().groups.empty()) {
							objects.back().groups.push_back(Group{});
						}

						std::string name;
						stream >> name;
						// TODO: Find material by name from .mtl
						objects.back().groups.back().material = std::make_shared<Material>(Material{ name });
					}
					else if (token == "f") {
						if (objects.back().groups.empty()) {
							objects.back().groups.push_back(Group{});
						}

						int vertexCount = 0;

						// vertexString is a string of the form "vertexIndex/textureIndex/normalIndex"
						std::string vertexString;

						while (stream >> vertexString) {
							vertexCount++;
							int vertexIndex = std::stoi(vertexString);
							
							// vertexIndex can be negative, which means it is relative to the end of the list
							if (vertexIndex < 0) {
								vertexIndex = (objects.back().vertices.size() - objects.back().extraVertexCount) + vertexIndex + 1;
							}

							int uvIndex = 0;
							int normalIndex = 0;

							int separatorIndex = vertexString.find('/');
							if (separatorIndex != std::string::npos) {
								vertexString = vertexString.substr(separatorIndex + 1);
								if (vertexString[0] != '/') {
									uvIndex = std::stoi(vertexString);
									if (uvIndex < 0) {
										uvIndex = objects.back().tempUVs.size() + uvIndex + 1;
									}
								}

								separatorIndex = vertexString.find('/');
								if (separatorIndex != std::string::npos) {
									normalIndex = std::stoi(vertexString.substr(separatorIndex + 1));
									if (normalIndex < 0) {
										normalIndex = objects.back().tempNormals.size() + normalIndex + 1;
									}
								}
							}

							int localVertexIndex = vertexIndex - 1 - objects.back().vertexOffset + objects.back().extraVertexCount;

							Vertex& vert = objects.back().vertices[localVertexIndex];
							Vertex newVertex = vert;

							if (uvIndex > 0) {
								newVertex.uv = objects.back().tempUVs[uvIndex - 1];
							}
							if (normalIndex > 0) {
								newVertex.normal = objects.back().tempNormals[normalIndex - 1];
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
										int duplicateVertexIndex = objects.back().duplicates[localVertexIndex];
										if (duplicateVertexIndex == -1) {
											// there is no vertex with the same position and texture
											// create a new vertex
											objects.back().vertices.push_back(newVertex);
											objects.back().duplicates.push_back(-1);
											objects.back().duplicates[localVertexIndex] = objects.back().vertices.size() - 1;
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
					}

				}
			}

	};

}
