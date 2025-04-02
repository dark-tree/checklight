//
// Created by tymon on 26/02/2025.
//

#ifndef CHECKLIGHT_PHYSICSELEMENT_HPP
#define CHECKLIGHT_PHYSICSELEMENT_HPP
#include "external.hpp"

class PhysicsElement {
protected:
    glm::vec3 position; ///< Position of the object in 3D space as a 3-dimensional vector
    glm::vec3 velocity; ///< Velocity of the object in 3D space as a 3-dimensional vector
    glm::quat rotation; ///< Rotation of the object in 3D space as a quaternion
    glm::vec3 angular_velocity; ///< angular_velocity of the object in 3D space as a 3-dimensional vector. Direction represents the axis of rotation, magnitude represents the speed of rotation (radians/s)
    glm::vec3 center_of_mass; ///< Center of mass of the object in 3D space as a 3-dimensional vector
    std::vector<glm::vec3> vertices; ///< Vertices forming the shape of the object's collider (point 0, 0, 0 must be contained withing the shape)
    std::vector<glm::ivec3> triangles; ///< Faces of the object's collider as triplets of vertices indexes (point 0, 0, 0 must be contained withing the shape)
    bool is_static; ///< Whether the object can be moved by external forces
    bool is_sphere; ///< Whether the collider of the object is to be a perfect sphere, if yes vertices and triangles will be disregarded in operations
    float gravity_scale; ///< Value by which gravity's acceleration is multiplied
    float sphere_collider_radius; ///< Radius of the simple sphere collider encompassing object's collider, used for initial collision checks
    float mass; ///< Mass of the object
    glm::mat3x3 inertia_tensor; ///< Moment of inertia of the object
    float coefficient_of_friction; ///< The coefficient of friction used for collision calculations (0 - no friction, 1 - instant stop)
    float coefficient_of_restitution; ///< The coefficient of restitution (bounciness) for collision calculations (0 - perfectly inelastic, 1 - perfectly elastic)
public:
    /** Called for physics updates
     * @param time_step length of time between the last 2 frames (const for physics update)
     * @param gravity gravitational acceleration strength
     */
    void update(float time_step, glm::vec3 gravity)
    {
        //integrate position over time with respect to acceleration
        velocity += gravity * (gravity_scale / 2.0f);
        position += velocity * time_step;
        position += gravity * (gravity_scale / 2.0f);

        //apply angular velocity
        rotation += (0.5f * rotation * glm::quat(0.0, angular_velocity) * time_step);
        rotation = glm::normalize(rotation);
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
        center_of_mass = findCenterOfMass();
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

    /// Sets the rotation of the object
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

    /// Sets the angular velocity of the object
    void setAngularVelocity(glm::vec3& vec3) {
        angular_velocity = vec3;
    }

    /// Sets the angular velocity of the object
    void setAngularVelocity(float x, float y, float z)
    {
        this->angular_velocity = glm::vec3(x, y, z);
    }

    /// Returns the rotation of an object
    glm::quat getAngularVelocity()
    {
        return angular_velocity;
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

    /** Sets the center of mass of the object
     * @warning Incorrectly setting the center of mass can result in horrible force application miscalculations
     */
    void setCenterOfMass(glm::vec3& vec3) {
        center_of_mass = vec3;
    }

    /** Sets the center of mass of the object
     * @warning Incorrectly setting the center of mass can result in horrible force application miscalculations
     */
    void setCenterOfMass(float x, float y, float z)
    {
        this->center_of_mass = glm::vec3(x, y, z);
    }

    /// Returns the center of mass of the object
    glm::vec3 getCenterOfMass()
    {
        return center_of_mass;
    }

    /// Sets the mass of an object
    void setMass(float mass)
    {
        this->mass = mass;
    }

    /// Returns the mass of an object
    float getMass()
    {
        return mass;
    }

    /// Sets the moment of inertia of an object
    void setMomentOfInertia(float moi[3][3])
    {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                inertia_tensor[i][j] = moi[i][j];
            }
        }
    }

    /// Sets the moment of inertia of an object
    void setMomentOfInertia(glm::mat3x3 moi)
    {
        inertia_tensor = moi;
    }

    /// Returns the mass of an object
    glm::mat3x3 getMomentOfInertia()
    {
        return inertia_tensor;
    }

    /// Sets the coefficient of friction of an object
    void setCoefficientOfFriction(float cof)
    {
        coefficient_of_friction = cof;
    }

    /// Returns the mass of an object
    float getCoefficientOfFriction()
    {
        return coefficient_of_friction;
    }

    /// Sets the coefficient of restitution of an object
    void setCoefficientOfRestitution(float cor)
    {
        coefficient_of_restitution = cor;
    }

    /// Returns the coefficient of restitution of an object
    float getCoefficientOfRestitution()
    {
        return coefficient_of_restitution;
    }

    /// Returns the furthest point from object origin in a given direction
    glm::vec3 furthestPoint(glm::vec3 direction)
    {
        if (is_sphere)
        {
            return direction;
        }

        double dot_result = -INFINITY;
        glm::vec3 returnal;
        for (glm::vec3 vertex : vertices)
        {
            glm::vec3 rotated = glm::rotate(rotation, vertex);
            double dr = glm::dot(rotated, direction);
            if (dr > dot_result)
            {
                dot_result = dr;
                returnal = rotated;
            }
        }
        return returnal + position;
    }

protected:
    /// Used to calculate center of mass of an object. Assumes uniform mass distribution
    glm::vec3 findCenterOfMass()
    {
        double total_volume = 0;
        glm::vec3 center = glm::vec3 (0, 0, 0);

        for (glm::ivec3 triangle : triangles)
        {
            //get vertices of a given face
            glm::vec3 v1 = vertices[triangle[0]];
            glm::vec3 v2 = vertices[triangle[1]];
            glm::vec3 v3 = vertices[triangle[2]];

            //Compute volume of a tetrahedron made of given face and origin point(0, 0, 0)
            //                          1 | v1x v1y v1z |
            // given by the formula V = — | v2x v2y v2z |
            //                          6 | v3x v3y v3z |
            float volume = (v1[0]*(v2[1]*v3[2] - v2[2]*v3[1])
                           - v1[1]*(v2[0]*v3[2] - v2[2]*v3[0])
                           + v1[2]*(v2[0]*v3[1] - v2[1]*v3[0])) / 6.0;

            //compute center of given tetrahedron
            glm::vec3 centroid = glm::vec3((v1[0] + v2[0] + v3[0]) / 4.0, (v1[1] + v2[1] + v3[1]) / 4.0, (v1[2] + v2[2] + v3[2]) / 4.0);

            //update volume and center of mass
            total_volume += volume;
            center += centroid * volume;
        }
        //normalize the center of mass by total volume
        center /= total_volume;

        return center;
    }

    float findMass()
    {
        //we will assume that the mass is evenly distributed and proportional to the volume
        double total_mass = 0;
        glm::vec3 offset = vertices[0];

        for (glm::ivec3 triangle : triangles)
        {
            //get vertices of a given face offset in a way to make origin fall on 1 of the points of the polygon
            //While calculating the volume of the tetrahedrons it didn't matter whether the origin fell outside the polygon
            //due to the "force of pull" of each polygon being additive with the distance from origin, but now the added
            //offset could cause us to greatly misinterpret the result. By assuming the shape is convex and knowing
            //the origin falls within the polygon, we can be certain the volume calculations are correct and as such can
            //repurpose this piece of code.
            glm::vec3 v1 = vertices[triangle[0]] - offset;
            glm::vec3 v2 = vertices[triangle[1]] - offset;
            glm::vec3 v3 = vertices[triangle[2]] - offset;

            //Compute volume of a tetrahedron made of given face and origin point(0, 0, 0)
            //                          1 | v1x v1y v1z |
            // given by the formula V = — | v2x v2y v2z |
            //                          6 | v3x v3y v3z |
            float volume = (v1[0]*(v2[1]*v3[2] - v2[2]*v3[1])
                            - v1[1]*(v2[0]*v3[2] - v2[2]*v3[0])
                            + v1[2]*(v2[0]*v3[1] - v2[1]*v3[0])) / 6.0;

            //update volume and mass
            total_mass += volume;
        }

        return total_mass;
    }

    float findMomentOfInertia()
    {
        //TODO THIS MATHEMATICAL HELL
        return 0;
    }

};


#endif //CHECKLIGHT_PHYSICSELEMENT_HPP
