#include "collider.hpp"

Collider::Collider() {
    vertices = std::vector<glm::vec3>();
    triangles = std::vector<glm::ivec3>();
//    volume = findVolume();
//    center_of_mass = findCenterOfMass();
//    inertia_tensor = findInertiaTensor();
//    calculateSphereColliderRadius();
}

Collider Collider::getCube()
{
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
    cube.volume = cube.findVolume();
    cube.center_of_mass = cube.findCenterOfMass();
    cube.inertia_tensor = cube.findInertiaTensor();
    cube.calculateSphereColliderRadius();
    return cube;
}

std::vector<glm::vec3> Collider::getVertices(){
    return vertices;
}

void Collider::setVertices(std::vector<glm::vec3> vertices){
    this->vertices = vertices;
    center_of_mass = findCenterOfMass();
    volume = findVolume();
    inertia_tensor = findInertiaTensor();
    calculateSphereColliderRadius();
}

std::vector<glm::ivec3> Collider::getTriangles(){
    return triangles;
}

void Collider::setTriangles(std::vector<glm::ivec3> triangles){
    this->triangles = triangles;
}

void Collider::LoadFromModel(std::shared_ptr<RenderMesh>){
    //TODO replace this with actual code
    this->vertices = Collider::getCube().getVertices();
    this->triangles = Collider::getCube().getTriangles();
    center_of_mass = findCenterOfMass();
    volume = findVolume();
    inertia_tensor = findInertiaTensor();
    calculateSphereColliderRadius();
}

void Collider::calculateSphereColliderRadius(){
    sphere_collider_radius = 0;
    for (glm::vec3 vec3 : vertices)
    {
        if (glm::length(vec3) > sphere_collider_radius)
        {
            sphere_collider_radius = glm::length(vec3);
        }
    }
}

void Collider::setAutoCenter(){
    center_of_mass = findCenterOfMass();
}


void Collider::setCenterOfMass(glm::vec3 center_of_mass)
{
    this->center_of_mass = center_of_mass;
}

void Collider::setCenterOfMass(float x, float y, float z) {
    this->center_of_mass = glm::vec3(x, y, z);
}

void Collider::setInertiaTensor(glm::mat3x3 inertia_tensor)
{
    this->inertia_tensor = inertia_tensor;
}

glm::vec3 Collider::getCenterOfMass()
{
    return center_of_mass;
}

glm::mat3x3 Collider::getInertiaTensor()
{
    return inertia_tensor;
}

float Collider::getSphereColliderRadious()
{
    return sphere_collider_radius;
}

float Collider::getVolume(){
    return volume;
}

glm::vec3 Collider::findCenterOfMass()
{
    double total_volume = 0;
    glm::vec3 center = glm::vec3 (0, 0, 0);

    for (glm::ivec3 triangle : triangles)
    {
        //get vertices of a given face
        glm::vec3 v1 = vertices[triangle[0]];
        glm::vec3 v2 = vertices[triangle[1]];
        glm::vec3 v3 = vertices[triangle[2]];

        //Compute local_volume of a tetrahedron made of given face and origin point(0, 0, 0)
        //                          1 | v1x v1y v1z |
        // given by the formula V = — | v2x v2y v2z |
        //                          6 | v3x v3y v3z |
        float local_volume = (v1[0] * (v2[1] * v3[2] - v2[2] * v3[1])
                              - v1[1]*(v2[0]*v3[2] - v2[2]*v3[0])
                              + v1[2]*(v2[0]*v3[1] - v2[1]*v3[0])) / 6.0;

        //compute center of given tetrahedron
        glm::vec3 centroid = glm::vec3((v1[0] + v2[0] + v3[0]) / 4.0, (v1[1] + v2[1] + v3[1]) / 4.0, (v1[2] + v2[2] + v3[2]) / 4.0);

        //update local_volume and center of mass
        total_volume += local_volume;
        center += centroid * local_volume;
    }
    //normalize the center of mass by total volume
    center /= total_volume;

    return center;
}

float Collider::findVolume()
{
    //we will assume that the mass is evenly distributed and proportional to the volume
    double total_volume = 0;
    glm::vec3 offset = vertices[0];

    for (glm::ivec3 triangle : triangles)
    {
        //get vertices of a given face offset in a way to make origin fall on 1 of the points of the polygon
        //While calculating the local_volume of the tetrahedrons it didn't matter whether the origin fell outside the polygon
        //due to the "force of pull" of each polygon being additive with the distance from origin, but now the added
        //offset could cause us to greatly misinterpret the result. By assuming the shape is convex and knowing
        //the origin falls within the polygon, we can be certain the local_volume calculations are correct and as such can
        //repurpose this piece of code.
        glm::vec3 v1 = vertices[triangle[0]] - offset;
        glm::vec3 v2 = vertices[triangle[1]] - offset;
        glm::vec3 v3 = vertices[triangle[2]] - offset;

        //Compute local_volume of a tetrahedron made of given face and origin point(0, 0, 0)
        //                          1 | v1x v1y v1z |
        // given by the formula V = — | v2x v2y v2z |
        //                          6 | v3x v3y v3z |
        float local_volume = (v1[0] * (v2[1] * v3[2] - v2[2] * v3[1])
                              - v1[1]*(v2[0]*v3[2] - v2[2]*v3[0])
                              + v1[2]*(v2[0]*v3[1] - v2[1]*v3[0])) / 6.0;

        //update local_volume and mass
        total_volume += local_volume;
    }

    return total_volume;
}

glm::mat3x3 Collider::findInertiaTensor() {
    //TODO THIS MATHEMATICAL HELL
    return glm::mat3x3(volume);
}