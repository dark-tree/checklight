//
// Created by tymon on 26/02/2025.
//

#ifndef CHECKLIGHT_PHYSICSENGINE_HPP
#define CHECKLIGHT_PHYSICSENGINE_HPP

#define TIMESTEP 0.020

#include "PhysicsElement.hpp"

class PhysicsEngine {
protected:
    std::vector<PhysicsElement> elements; /// List of objects currently existing in the scene
    float gravity_strength; /// Acceleration due to gravity

public:
    /**
     * A single update step to the physics calculations. Moves objects according to their speed and gravity, detects collisions and applies forces in case of one.
     * @returns difference between time step and calculation time
     */
    double physicsUpdate()
    {
        //timer start
        auto start = std::chrono::system_clock::now();
        //TODO get list of updatable elements
        //update all existing objects
        for (PhysicsElement element : elements)
        {
            element.update(TIMESTEP, gravity_strength);
        }
        //check for collision between every object pair
        for (int i = 0; i < elements.size() - 1; i++)
        {
            for (int j = i + 1; j < elements.size(); j++)
            {
                //initial, time efficient, but inaccurate collision detection
                if (initialCollisionCheck(elements[i], elements[j]))
                {
                    //second, more time-consuming, but exact detection
                    if (gilbertJohnsonKeerthi(elements[i], elements[j]))
                    {
                        applyForces(elements[i], elements[j]);
                        //TODO on collision function in pawn
                    }
                }
            }
        }
        //timer end
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_time = end-start;
        return TIMESTEP - elapsed_time.count();
    }

    bool initialCollisionCheck(PhysicsElement& a, PhysicsElement& b)
    {
        return (glm::length(a.getPosition() - b.getPosition()) <= a.getSphereColliderRadius() + b.getSphereColliderRadius());
    }

    bool gilbertJohnsonKeerthi(PhysicsElement& a, PhysicsElement& b)
    {
        //get direction by comparing relative position of objects
        glm::vec3 direction = glm::normalize(b.getPosition() - a.getPosition());
        //get the 0th point of the simplex by getting the support point of the Minkowski difference in the above direction
        std::vector<glm::vec3> simplex = {calculateSupport(a, b, direction)};
        //get new direction as a vector pointing from 0th point of simplex to the origin
        direction = glm::vec3(0, 0, 0) - simplex[0];
        while (true)
        {
            //get new support point
            glm::vec3 point = calculateSupport(a, b, direction);
            //if the new point does not pass the origin, the point is not valid, and so the collision didn't happen - return false
            if (glm::dot(point, direction) < 0)
            {
                return false;
            }
            //point valid - append it to simplex
            simplex.push_back(point);
            //make sure the origin is in the simplex, if it is the collision happened - return true
            //if it's not - update the simplex and continue
            if (manageSimplex(simplex, direction))
            {
                return true;
            }
        }
        return false;
    }

    glm::vec3 calculateSupport(PhysicsElement& a, PhysicsElement& b, glm::vec3 direction)
    {
        return a.furthestPoint(direction) - b.furthestPoint(-direction);
    }

    bool manageSimplex(std::vector<glm::vec3>& simplex, glm::vec3& direction)
    {
        if (simplex.size() == 2)
        {
            return lineCase(simplex, direction);
        }
        else if (simplex.size() == 3)
        {
            return planeCase(simplex, direction);
        }
        return tetrahedronCase(simplex, direction);
    }

    bool lineCase(std::vector<glm::vec3>& simplex, glm::vec3& direction)
    {
        //TODO
        return false;
    }

    bool planeCase(std::vector<glm::vec3>& simplex, glm::vec3& direction)
    {
        //TODO
        return false;
    }

    bool tetrahedronCase(std::vector<glm::vec3>& simplex, glm::vec3& direction)
    {
        //TODO
        return false;
    }

    void applyForces(PhysicsElement& a, PhysicsElement& b)
    {
        //TODO this whole thing
    }

    std::vector<PhysicsElement> getElements()
    {
        return {};
    }
};


#endif //CHECKLIGHT_PHYSICSENGINE_HPP
