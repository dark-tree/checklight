#pragma once

#include "external.hpp"

class PhysicsElement {
public:
    glm::vec3 position; ///< Position of the object in 3D space as a 3-dimensional vector
    glm::vec3 velocity; ///< Velocity of the object in 3D space as a 3-dimensional vector
    glm::quat rotation; ///< Rotation of the object in 3D space as a quaternion
    glm::vec3 angular_velocity; ///< angular_velocity of the object in 3D space as a 3-dimensional vector. Direction represents the axis of rotation, magnitude represents the speed of rotation (radians/s)
    glm::vec3 center_of_mass; ///< Center of mass of the object in 3D space as a 3-dimensional vector
    std::vector<glm::vec3> vertices; ///< Vertices forming the shape of the object's collider (point 0, 0, 0 must be contained withing the shape)
    std::vector<glm::ivec3> triangles; ///< Faces of the object's collider as triplets of vertices indexes (point 0, 0, 0 must be contained withing the shape)
    bool is_static; ///< Whether the object can be moved by external forces
    glm::vec3 gravity_scale; ///< Value by which gravity's acceleration is multiplied
    float sphere_collider_radius; ///< Radius of the simple sphere collider encompassing object's collider, used for initial collision checks
    float mass; ///< Mass of the object
    glm::mat3x3 inertia_tensor; ///< Moment of inertia of the object
    float coefficient_of_friction; ///< The coefficient of friction used for collision calculations (0 - no friction, 1 - instant stop)
    float coefficient_of_restitution; ///< The coefficient of restitution (bounciness) for collision calculations (0 - perfectly inelastic, 1 - perfectly elastic)

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

    glm::vec3 furthestPoint(glm::vec3 direction)
    {
        double dot_result = -INFINITY;
        glm::vec3 returnal {0, 0, 0};
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
};
