#pragma once
#include "external.hpp"

class Collider{
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::ivec3> triangles;

public:
    Collider(){
        vertices = std::vector<glm::vec3>();
        triangles = std::vector<glm::ivec3>();
    }

    static Collider getCube(){
        Collider cube;
        cube.vertices = {
                {-1.f, -1.f, 1.f},
                {1.f, -1.f, 1.f},
                {1.f, 1.f, 1.f},
                {-1.f, 1.f, 1.f},
                {1.f, -1.f, -1.f},
                {-1.f, -1.f, -1.f},
                {-1.f, 1.f, -1.f},
                {1.f, 1.f, -1.f}
        };
        cube.triangles = {
                {0, 1, 2},
                {0, 2, 3},
                {1, 4, 7},
                {1, 7, 2},
                {3, 2, 7},
                {3, 7, 6},
                {4, 5, 6},
                {4, 6, 7},
                {5, 0, 3},
                {5, 3, 6},
                {0, 5, 4},
                {0, 4, 1}
        };
        return cube;
    }

    std::vector<glm::vec3> getVertices(){
        return vertices;
    }

    void setVertices(std::vector<glm::vec3> vertices){
        this->vertices = vertices;
    }

    std::vector<glm::ivec3> getTriangles(){
        return triangles;
    }

    void setTriangles(std::vector<glm::ivec3> triangles){
        this->triangles = triangles;
    }
};