//
// Created by tymon on 26/02/2025.
//

#ifndef CHECKLIGHT_PHYSICSENGINE_HPP
#define CHECKLIGHT_PHYSICSENGINE_HPP

#define TIMESTEP 0.020

#include "PhysicsElement.hpp"

class PhysicsEngine {
protected:

    float gravity_strength; /// Acceleration due to gravity

public:
    //this is here for testing only todo move into protected after testing
    std::vector<PhysicsElement> elements; /// List of objects currently existing in the scene
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

    /// Used to calculate a support point of the minkowski difference in a given direction
    glm::vec3 calculateSupport(PhysicsElement& a, PhysicsElement& b, glm::vec3 direction)
    {
        return a.furthestPoint(direction) - b.furthestPoint(-direction);
    }

    bool manageSimplex(std::vector<glm::vec3>& simplex, glm::vec3& direction)
    {
        if (simplex.size() > 4)
        {
            throw std::runtime_error {"Simplex has more than 4 vertexes!"};
        }
        //in each of those functions the points on the simplex are lettered from newest to oldest (a-most recently added, d-least recently added)
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
        //vector pointing from the newest point to the origin
        glm::vec3 ao = glm::vec3(0, 0, 0) - simplex[1];
        //vector pointing from the newest point to the oldest
        glm::vec3 ab = simplex[0] - simplex[1];

        //find a perpendicular vector from AB that points in the direction of origin using the triple product operation
        direction = tripleCrossProduct(ab, ao);
        //we have a line here, and we need a tetrahedron - another loop
        return false;
    }

    bool planeCase(std::vector<glm::vec3>& simplex, glm::vec3& direction)
    {
        //vector pointing from the newest point to the origin
        glm::vec3 ao = glm::vec3(0, 0, 0) - simplex[2];

        //vectors AB & AC
        glm::vec3 ab = simplex[1] - simplex[2];
        glm::vec3 ac = simplex[0] - simplex[2];
        //no need to test all cases of possible origin positions relative to the origin.
        //the origin can only exist above/below the triangle itself or the Voronoi regions behind edges AB & AC for reason I won't explain here

        //the triangle's normal vector
        glm::vec3 normal = glm::cross(ab, ac);

        //vector within the plane of the triangle pointing away from the edge AB & AC
        glm::vec3 voronoi_ab = glm::cross(ab, normal);
        glm::vec3 voronoi_ac = glm::cross(ac, -normal);

        //check whether the origin is located in the AB Voronoi region (or above/below it)
        if (glm::dot(voronoi_ab, ao) > 0)
        {
            //set the search direction as a perpendicular from edge ab to the origin
            direction = tripleCrossProduct(ab, ao);

            //update the triangle points, we're not yet ready to form a tetrahedron
            simplex.erase(simplex.begin());
            //while we technically could go to a tetrahedron instead of a triangle here, it'll make our life easier and the algorithm faster in most cases
        } //check whether the origin is located in the AC Voronoi region (or above/below it)
        else if (glm::dot(voronoi_ac, ao) > 0)
        {
            //same case as above
            direction = tripleCrossProduct(ac, ao);

            simplex.erase(simplex.begin() + 1);
        } //if we got here that means the origin is within the triangle, we just need to check whether above or below
        else if (glm::dot(normal, ao) >= 0)
        {
            //case for above
            //all that needs to be done is changing the search direction to the triangle's normal
            direction = normal;
        }
        else
        {
            //origin is below
            //update the search direction for the negative normal
            direction = -normal;
        }

        //we have a triangle here (or even a line), and we need a tetrahedron - another loop
        return false;
    }

    bool tetrahedronCase(std::vector<glm::vec3>& simplex, glm::vec3& direction)
    {
        //as in the cases above we start with defining vectors
        glm::vec3 ao = glm::vec3(0, 0, 0) - simplex[3];

        glm::vec3 ab = simplex[2] - simplex[3];
        glm::vec3 ac = simplex[1] - simplex[3];
        glm::vec3 ad = simplex[0] - simplex[3];

        glm::vec3 normal_abc = glm::cross(ab, ac);
        glm::vec3 normal_acd = glm::cross(ac, ad);
        glm::vec3 normal_adb = glm::cross(ad, ab);

        //check if the origin is outside each given face
        bool over_abc = glm::dot(normal_abc, ao) > 0;
        bool over_acd = glm::dot(normal_acd, ao) > 0;
        bool over_adb = glm::dot(normal_adb, ao) > 0;

        if (!over_abc && !over_acd && !over_adb)
        {
            //origin within all faces of the tetrahedron - we have a collision!
            return true;
        }
        else if (over_abc && !over_acd && !over_adb)
        {
            //proceed to check whether the origin is within the AB, AC or ABC voronoi region
            goto face_check1;
        }
        else if (!over_abc && over_acd && !over_adb)
        {
            //rotate ACD into ABC

            //point c into b & point d into c
            simplex[2] = simplex[1];
            simplex[1] = simplex[0];

            //rest is self-explanatory
            ab = ac;
            ac = ad;

            normal_abc = normal_acd;

            //after rotating we do the exact same as above
            goto face_check1;
        }
        else if (!over_abc && !over_acd && over_adb)
        {
            //rotate ADB into ABC

            //point b into c & point d into c
            simplex[1] = simplex[2];
            simplex[2] = simplex[0];

            //rest is self-explanatory
            ac = ab;
            ab = ad;

            normal_abc = normal_adb;

            //after rotating we do the exact same as above
            goto face_check1;
        }
        else if (over_abc && over_acd && !over_adb)
        {
            goto double_face_check;
        }
        else if (!over_abc && over_acd && over_adb)
        {
            //Rotate ACD into ABC and ADB into ACD
            //b goes into tmp
            glm::vec3 tmp = simplex[2];
            //c goes into b
            simplex[2] = simplex[1];
            //d goes into c
            simplex[1] = simplex[0];
            //b goes into d
            simplex[0] = tmp;

            //rest is self-explanatory
            tmp = ab;
            ab = ac;
            ac = ad;
            ad = tmp;

            normal_abc = normal_acd;
            normal_acd = normal_adb;

            goto double_face_check;
        }
        else if (over_abc && !over_acd && over_adb)
        {
            //Rotate ABD into ABC and ABC into ACD
            //c goes into tmp
            glm::vec3 tmp = simplex[1];
            //b into c
            simplex[1] = simplex[2];
            //d into b
            simplex[2] = simplex[0];
            //c into d
            simplex[0] = tmp;

            //rest is self-explanatory
            tmp = ac;
            ac = ab;
            ab = ad;
            ad = tmp;

            normal_acd = normal_abc;
            normal_abc = normal_adb;

            goto double_face_check;
        }
        else
        {
            //This is the edgiest of edge cases:
            //You have somehow constructed a tetrahedron of 4 points on a plane that DOES CONTAIN the origin.
            //That is technically a case where the 2 objects do touch but do not overlap.
            //That, or your point a lies EXACTLY at 0, 0, 0,
            //or you have constructed some other degenerate 4 vertex shape that's impossible for me to even wrap my head around.
            //Whichever the case - I congratulate you!
            //Have a collision as a treat - I'm returning true
            return true;
        }

        face_check1:
        //check if origin is in the AC Voronoi region
        if (glm::dot(glm::cross(normal_abc, ac), ao) > 0)
        {
            //this case means we must remove 2 points and go back to a triangle case
            //set the direction the same as in a triangle case
            direction = tripleCrossProduct(ac, ao);

            //remove b & d
            simplex.erase(simplex.begin() + 2);
            simplex.erase(simplex.begin());

            //no collision - return false
            return false;
        }

        face_check2:
        //check if origin is in the AB Voronoi region
        if (glm::dot(glm::cross(normal_abc, ac), ao) > 0)
        {
            //this case means we must remove 2 points and go back to a triangle case
            //set the direction the same as in a triangle case
            direction = tripleCrossProduct(ab, ao);

            //remove c & d
            simplex.erase(simplex.begin() + 1);
            simplex.erase(simplex.begin());

            //no collision - return false
            return false;
        }

        //if not in either AC nor AB it must be in ABC Voronoi region
        //we set ABC as the new triangle and continue as before
        direction = normal_abc;
        simplex.erase(simplex.begin());
        //still no collision
        return false;

        double_face_check:

        if (glm::dot(glm::cross(normal_abc, ac), ao) > 0)
        {
            //looking from ABC's perspective the origin is beyond AC
            //this means we only need to rerun the single face test for ACD

            //rotate ACD into ABC
            //rotate ACD into ABC

            //point c into b & point d into c
            simplex[2] = simplex[1];
            simplex[1] = simplex[0];

            //rest is self-explanatory
            ab = ac;
            ac = ad;

            normal_abc = normal_acd;

            goto face_check1;
        }

        //if we got here we know we can only be in the AB or ABC Voronoi region
        //as such all that is left to do is to rerun the 2nd half of 1 face test
        goto face_check2;

        return false;
    }

    glm::vec3 tripleCrossProduct(glm::vec3& a, glm::vec3& b)
    {
        return glm::cross(glm::cross(a, b), a);
    }

    void applyForces(PhysicsElement& a, PhysicsElement& b)
    {
        //TODO this whole thing
    }

    std::vector<PhysicsElement> getElements()
    {
        //todo
        return {};
    }
};


#endif //CHECKLIGHT_PHYSICSENGINE_HPP
