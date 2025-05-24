#pragma once
#include "external.hpp"
#include "render/api/mesh.hpp"

class Collider{
protected:
    std::vector<glm::vec3> vertices;
    std::vector<glm::ivec3> triangles;
    glm::vec3 center_of_mass;

    float sphere_collider_radious;
    glm::vec3 inertia_tensor;

    float volume;

    /// Finds the volume of an object, will be called if volumen is null
    float findVolume();

    /// Used to calculate center of mass of an object. Assumes uniform mass distribution
    glm::vec3 findCenterOfMass();

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

    void LoadFromModel(std::shared_ptr<RenderMesh>){
      //TODO replace this with actual code
      this->vertices = Collider::getCube().getVertices();
      this->triangles = Collider::getCube().getTriangles();
    }

    void calculateSphereColliderRadious(){
      //TODO
    }

    void setAutoCenter();

    /// Sets the center of mass of the object
    void setCenterOfMass(glm::vec3 center_of_mass);

    /// Sets the radius of the sphere collider
    void setSphereColliderRadious(float sphere_collider_radious);

    /// Sets the inertia tensor of the object
    void setInertiaTensor(glm::vec3 inertia_tensor);

    /// Gets the center of mass of the object
    glm::vec3 getCenterOfMass();

    /// Gets the inertia tensor of the object
    glm::vec3 getInertiaTensor();

    /// Gets the radius of the sphere collider
    float getSphereColliderRadious();

    /// Returns the volume of an object
    float getVolume();


};