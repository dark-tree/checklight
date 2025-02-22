#pragma once

#include "external.hpp"
#include "render/api/vertex.hpp"

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

	static std::map<std::string, std::shared_ptr<ObjMaterial>> open(std::string path);
};

struct ObjGroup {
	std::string name = "";
	std::vector<Index3D::type> indices;
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

		static std::string getMtllib(std::string path);
		static std::vector<ObjObject> open(std::string path, const std::map<std::string, std::shared_ptr<ObjMaterial>>& materials);
};
