//
// Created by tymon on 26/02/2025.
//

#ifndef CHECKLIGHT_PHYSICSENGINE_HPP
#define CHECKLIGHT_PHYSICSENGINE_HPP

#define TIMESTEP 0.020

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
        return false;
    }

    void applyForces(PhysicsElement& a, PhysicsElement& b)
    {

    }

    std::vector<PhysicsElement> getElements()
    {
        return {};
    }
};


#endif //CHECKLIGHT_PHYSICSENGINE_HPP
