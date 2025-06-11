#pragma once

#include "external.hpp"


class PhysicsElement;
class BoardManager;

struct SupportPoint {
    glm::vec3 point;    // Minkowski difference point
    glm::vec3 point_a;   // Support point on A
    glm::vec3 point_b;   // Support point on B
};

class PhysicsEngine {
protected:

    glm::vec3 gravity_strength; /// Acceleration due to gravity as a 3-dimensional vector

    std::vector<PhysicsElement> elements;

    BoardManager* boardManager;

    int frame_num = 0;



public:


    PhysicsEngine(glm::vec3 gravity_strength,BoardManager* skibidi);

    ~PhysicsEngine();

    void getElements();

    /**
     * A single update step to the physics calculations. Moves objects according to their speed and gravity, detects collisions and applies forces in case of one.
     * @returns difference between time step and calculation time
     */
    double physicsUpdate();

    void setGravityScale(const glm::vec3& gravityScale);

    bool initialCollisionCheck(PhysicsElement& a, PhysicsElement& b);

    std::pair<bool, std::vector<SupportPoint>> gilbertJohnsonKeerthi(PhysicsElement& a, PhysicsElement& b);

    /// Used to calculate a support point of the minkowski difference in a given direction
    glm::vec3 calculateSupport(PhysicsElement& a, PhysicsElement& b, glm::vec3& direction);

    bool manageSimplex(std::vector<SupportPoint>& simplex, glm::vec3& direction);

    bool lineCase(std::vector<SupportPoint>& simplex, glm::vec3& direction);

    bool planeCase(std::vector<SupportPoint>& simplex, glm::vec3& direction);

    bool tetrahedronCase(std::vector<SupportPoint>& simplex, glm::vec3& direction);

    /** Functional expansion to the GJK algorithm, used for finding the depth and normal of the collision
     * @param simplex end simplex of GJK Algorithm
     * @param a 1st colliding physics element
     * @param b 2nd colliding physics element
     * @return std::pair containing the depth of the collision, a glm::vec3 containing the normal of the collision and a glm::vec3 with the point of the collision in global space
     */
    std::pair<std::pair<float, glm::vec3>, glm::vec3> expandingPolytope(std::vector<SupportPoint>& simplex, PhysicsElement& a, PhysicsElement& b);

    /// Used to calculate a support point of the minkowski difference in a given direction, returns furthest points as well
    SupportPoint calculateSupportWithPoints(PhysicsElement& a, PhysicsElement& b, glm::vec3& direction);

    /// Function for returning the normalized normals of given faces of a polytope
    std::pair<std::vector<glm::vec4>, int> getFaceNormals(std::vector<SupportPoint>& polytope, std::vector<glm::ivec3>& faces);

    void addUniqueEdge(std::vector<std::pair<int, int>>& edges, std::vector<glm::ivec3> faces, int face_num, int a, int b);

    void applyForces(PhysicsElement& a, PhysicsElement& b, float collision_depth, glm::vec3& collision_normal, glm::vec3& collision_point);
};

