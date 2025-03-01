//
// Created by tymon on 26/02/2025.
//

#ifndef CHECKLIGHT_PHYSICSELEMENT_HPP
#define CHECKLIGHT_PHYSICSELEMENT_HPP
#include "external.hpp"

class PhysicsElement {
protected:
    glm::vec3 position; /// Position of the object in 3D space as a 3-dimensional vector
    glm::vec3 velocity; /// Velocity of the object in 3D space as a 3-dimensional vector
    glm::quat rotation; /// Rotation of the object in 3D space as a quaternion
    std::vector<glm::vec3> vertices; /// Vertices forming the shape of the object's collider
    std::vector<glm::ivec3> triangles; /// Faces of the object's collider as triplets of vertices indexes
    bool is_static; /// Whether the object can be moved by external forces
    float gravity_scale; /// Value by which gravity's acceleration is multiplied
    float sphere_collider_radius; /// Radius of the simple sphere collider encompassing object's collider, used for initial collision checks
public:
    /** Called for physics updates
     * @param time_step length of time between the last 2 frames (const for physics update)
     * @param gravity gravitational acceleration strength
     */
    void update(float time_step, float gravity)
    {
        position += velocity * time_step;
        position += glm::vec3(0, -gravity * gravity_scale, 0);
    }

    PhysicsElement()
    {
        position = glm::vec3(0,0,0);
        velocity = glm::vec3(0,0,0);
        rotation = glm::quat();
        vertices = {
                {-1.f, -1.f, 1.f},
                {1.f, -1.f, 1.f},
                {1.f, 1.f, 1.f},
                {-1.f, 1.f, 1.f},
                {1.f, -1.f, -1.f},
                {-1.f, -1.f, -1.f},
                {-1.f, 1.f, -1.f},
                {1.f, 1.f, -1.f}
        };
        triangles = {
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
        is_static = false;
        gravity_scale = 1;
        sphere_collider_radius = 0;
        for (glm::vec3 vec3 : vertices)
        {
            if (glm::length(vec3) > sphere_collider_radius)
            {
                sphere_collider_radius = glm::length(vec3);
            }
        }
    }

    /// Sets the vec3 of the object
    void setPosition(glm::vec3& vec3)
    {
        this->position = vec3;
    }

    /// Sets the position of the object
    void setPosition(float x, float y, float z)
    {
        this->position = glm::vec3(x, y, z);
    }

    /// Returns the position of an object
    glm::vec3 getPosition()
    {
        return position;
    }

    /// Sets the vec3 of the object
    void setVelocity(glm::vec3& vec3)
    {
        this->velocity = vec3;
    }

    /// Sets the position of the object
    void setVelocity(float x, float y, float z)
    {
        this->velocity = glm::vec3(x, y, z);
    }

    /// Returns the velocity of an object
    glm::vec3 getVelocity()
    {
        return velocity;
    }

    /// Sets the quat of the object
    void setRotation(glm::quat& quat)
    {
        this->rotation = quat;
    }

    /// Sets the rotation of the object
    void setRotation(float w, float x, float y, float z)
    {
        this->rotation = glm::quat(w, x, y, z);
    }

    /// Returns the rotation of an object
    glm::quat getRotation()
    {
        return rotation;
    }

    /// Sets the is_static property of the object (whether it is affected by external forces)
    void setIsStatic(bool boo)
    {
        is_static = boo;
    }

    /// Returns the is_static property of the object (whether it is affected by external forces)
    bool getIsStatic()
    {
        return is_static;
    }

    /// Sets the gravity scale of an object
    void setGravityScale(float gs)
    {
        gravity_scale = gs;
    }

    /// Returns the gravity scale of an object
    float getGravityScale()
    {
        return gravity_scale;
    }

    /// Sets the shape of the collider of the object and recalculates its sphere collider radius
    void setCollider(std::vector<glm::vec3>& vertexes, std::vector<glm::ivec3>& faces)
    {
        vertices = vertexes;
        triangles = faces;
        sphere_collider_radius = 0;
        for (glm::vec3 vec3 : vertices)
        {
            if (glm::length(vec3) > sphere_collider_radius)
            {
                sphere_collider_radius = glm::length(vec3);
            }
        }
    }

    /// Returns the object vertices
    std::vector<glm::vec3> getColliderVertices()
    {
        return vertices;
    }

    /// Returns the objects faces
    std::vector<glm::ivec3> getColliderTriangles()
    {
        return triangles;
    }

    /// Returns the radius of the sphere collider inscribed onto an object
    float getSphereColliderRadius()
    {
        return sphere_collider_radius;
    }

    /// Returns the furthest point from object origin in a given direction
    glm::vec3 furthestPoint(glm::vec3 direction)
    {
        //TODO factor in rotation
        //TODO sphere case
        double dot_result = -INFINITY;
        glm::vec3 returnal;
        for (glm::vec3 vertex : vertices)
        {
            double dr = glm::dot(vertex, direction);
            if (dr > dot_result)
            {
                dot_result = dr;
                returnal = vertex;
            }
        }
        return returnal + position;
    }
};


#endif //CHECKLIGHT_PHYSICSELEMENT_HPP
