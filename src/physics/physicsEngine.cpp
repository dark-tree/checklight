#include "physicsEngine.hpp"
#include "engine/board.hpp"
#include "engine/boardManager.hpp"
#include "shared/math.hpp"
#include "physicsElement.hpp"
#include "engine/entity/component/physics.hpp"


PhysicsEngine::PhysicsEngine(glm::vec3 gravity_strength, BoardManager *skibidi) {
	elements = std::vector<PhysicsElement>();
	this->gravity_strength = gravity_strength;
	boardManager = skibidi;
}

PhysicsEngine::~PhysicsEngine() {

}


bool PhysicsEngine::initialCollisionCheck(PhysicsElement &a, PhysicsElement &b) {
	return (glm::length(a.position - b.position) <= a.sphere_collider_radius + b.sphere_collider_radius);
}

std::pair<bool, std::vector<SupportPoint> > PhysicsEngine::gilbertJohnsonKeerthi(PhysicsElement &a, PhysicsElement &b) {
	//get direction by comparing relative position of objects
	glm::vec3 direction = glm::normalize(b.position - a.position);
	//get the 0th point of the simplex by getting the support point of the Minkowski difference in the above direction
	std::vector<SupportPoint> simplex;
    simplex.push_back(calculateSupportWithPoints(a, b, direction));
	//get new direction as a vector pointing from 0th point of simplex to the origin
	direction = glm::vec3(0, 0, 0) - simplex.at(0).point;
	while (true) {
		//get new support point
        SupportPoint point = calculateSupportWithPoints(a, b, direction);
		//if the new point does not pass the origin, the point is not valid, and so the collision didn't happen - return false
		if (glm::dot(point.point, direction) < 0) {
			return {false, simplex};
		}
		//point valid - append it to simplex
		simplex.push_back(point);
		//make sure the origin is in the simplex, if it is the collision happened - return true
		//if it's not - update the simplex and continue
		if (manageSimplex(simplex, direction)) {
			return {true, simplex};
		}
	}
	return {false, simplex};
}

glm::vec3 PhysicsEngine::calculateSupport(PhysicsElement &a, PhysicsElement &b, glm::vec3 &direction) {
	return a.furthestPoint(direction) - b.furthestPoint(-direction);
}

bool PhysicsEngine::manageSimplex(std::vector<SupportPoint> &simplex, glm::vec3 &direction) {
	if (simplex.size() > 4) {
		throw std::runtime_error{"Simplex has more than 4 vertexes!"};
	}
	//in each of those functions the points on the simplex are lettered from newest to oldest (a-most recently added, d-least recently added)
	if (simplex.size() == 2) {
		return lineCase(simplex, direction);
	} else if (simplex.size() == 3) {
		return planeCase(simplex, direction);
	}
	return tetrahedronCase(simplex, direction);
}

bool PhysicsEngine::lineCase(std::vector<SupportPoint> &simplex, glm::vec3 &direction) {
	//vector pointing from the newest point to the origin
	glm::vec3 ao = glm::vec3(0, 0, 0) - simplex.at(1).point;
	//vector pointing from the newest point to the oldest
	glm::vec3 ab = simplex.at(0).point - simplex.at(1).point;

	//find a perpendicular vector from AB that points in the direction of origin using the triple product operation
	direction = math::tripleCrossProduct(ab, ao);
	//we have a line here, and we need a tetrahedron - another loop
	return false;
}

bool PhysicsEngine::planeCase(std::vector<SupportPoint> &simplex, glm::vec3 &direction) {
	//vector pointing from the newest point to the origin
	glm::vec3 ao = glm::vec3(0, 0, 0) - simplex.at(2).point;

	//vectors AB & AC
	glm::vec3 ab = simplex.at(1).point - simplex.at(2).point;
	glm::vec3 ac = simplex.at(0).point - simplex.at(2).point;
	//no need to test all cases of possible origin positions relative to the origin.
	//the origin can only exist above/below the triangle itself or the Voronoi regions behind edges AB & AC for reason I won't explain here

	//the triangle's normal vector
	glm::vec3 normal = glm::cross(ab, ac);

	//vector within the plane of the triangle pointing away from the edge AB & AC
	glm::vec3 voronoi_ab = glm::cross(ab, normal);
	glm::vec3 voronoi_ac = glm::cross(ac, -normal);

	//check whether the origin is located in the AB Voronoi region (or above/below it)
	if (glm::dot(voronoi_ab, ao) > 0) {
		//set the search direction as a perpendicular from edge ab to the origin
		direction = math::tripleCrossProduct(ab, ao);

		//update the triangle points, we're not yet ready to form a tetrahedron
		simplex.erase(simplex.begin());
		//while we technically could go to a tetrahedron instead of a triangle here, it'll make our life easier and the algorithm faster in most cases
	} //check whether the origin is located in the AC Voronoi region (or above/below it)
	else if (glm::dot(voronoi_ac, ao) > 0) {
		//same case as above
		direction = math::tripleCrossProduct(ac, ao);

		simplex.erase(simplex.begin() + 1);
	} //if we got here that means the origin is within the triangle, we just need to check whether above or below
	else if (glm::dot(normal, ao) >= 0) {
		//case for above
		//all that needs to be done is changing the search direction to the triangle's normal
		direction = normal;
	} else {
		//origin is below
		//update the search direction for the negative normal
		direction = -normal;
	}

	//we have a triangle here (or even a line), and we need a tetrahedron - another loop
	return false;
}

bool PhysicsEngine::tetrahedronCase(std::vector<SupportPoint> &simplex, glm::vec3 &direction) {
	//as in the cases above we start with defining vectors
	glm::vec3 ao = glm::vec3(0, 0, 0) - simplex.at(3).point;

	glm::vec3 ab = simplex.at(2).point - simplex.at(3).point;
	glm::vec3 ac = simplex.at(1).point - simplex.at(3).point;
	glm::vec3 ad = simplex.at(0).point - simplex.at(3).point;

	glm::vec3 normal_abc = glm::cross(ab, ac);
	glm::vec3 normal_acd = glm::cross(ac, ad);
	glm::vec3 normal_adb = glm::cross(ad, ab);

	//check if the origin is outside each given face
	bool over_abc = glm::dot(normal_abc, ao) > 0;
	bool over_acd = glm::dot(normal_acd, ao) > 0;
	bool over_adb = glm::dot(normal_adb, ao) > 0;

	if (!over_abc && !over_acd && !over_adb) {
		//origin within all faces of the tetrahedron - we have a collision!
		return true;
	} else if (over_abc && !over_acd && !over_adb) {
		//proceed to check whether the origin is within the AB, AC or ABC voronoi region
		goto face_check1;
	} else if (!over_abc && over_acd && !over_adb) {
		//rotate ACD into ABC

		//point c into b & point d into c
		simplex.at(2) = simplex.at(1);
		simplex.at(1) = simplex.at(0);

		//rest is self-explanatory
		ab = ac;
		ac = ad;

		normal_abc = normal_acd;

		//after rotating we do the exact same as above
		goto face_check1;
	} else if (!over_abc && !over_acd && over_adb) {
		//rotate ADB into ABC

		//point b into c & point d into c
		simplex.at(1) = simplex.at(2);
		simplex.at(2) = simplex.at(0);

		//rest is self-explanatory
		ac = ab;
		ab = ad;

		normal_abc = normal_adb;

		//after rotating we do the exact same as above
		goto face_check1;
	} else if (over_abc && over_acd && !over_adb) {
		goto double_face_check;
	} else if (!over_abc && over_acd && over_adb) {
		//Rotate ACD into ABC and ADB into ACD
		//b goes into tmp
        SupportPoint tmp = simplex.at(2);
		//c goes into b
		simplex.at(2) = simplex.at(1);
		//d goes into c
		simplex.at(1) = simplex.at(0);
		//b goes into d
		simplex.at(0) = tmp;

		//rest is self-explanatory
		tmp.point = ab;
		ab = ac;
		ac = ad;
		ad = tmp.point;

		normal_abc = normal_acd;
		normal_acd = normal_adb;

		goto double_face_check;
	} else if (over_abc && !over_acd && over_adb) {
		//Rotate ABD into ABC and ABC into ACD
		//c goes into tmp
        SupportPoint tmp = simplex.at(1);
		//b into c
		simplex.at(1) = simplex.at(2);
		//d into b
		simplex.at(2) = simplex.at(0);
		//c into d
		simplex.at(0) = tmp;

		//rest is self-explanatory
		tmp.point = ac;
		ac = ab;
		ab = ad;
		ad = tmp.point;

		normal_acd = normal_abc;
		normal_abc = normal_adb;

		goto double_face_check;
	} else {
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
	if (glm::dot(glm::cross(normal_abc, ac), ao) > 0) {
		//this case means we must remove 2 points and go back to a triangle case
		//set the direction the same as in a triangle case
		direction = math::tripleCrossProduct(ac, ao);

		//remove b & d
		simplex.erase(simplex.begin() + 2);
		simplex.erase(simplex.begin());

		//no collision - return false
		return false;
	}

face_check2:
	//check if origin is in the AB Voronoi region
	if (glm::dot(glm::cross(normal_abc, ac), ao) > 0) {
		//this case means we must remove 2 points and go back to a triangle case
		//set the direction the same as in a triangle case
		direction = math::tripleCrossProduct(ab, ao);

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

	if (glm::dot(glm::cross(normal_abc, ac), ao) > 0) {
		//looking from ABC's perspective the origin is beyond AC
		//this means we only need to rerun the single face test for ACD

		//rotate ACD into ABC
		//rotate ACD into ABC

		//point c into b & point d into c
		simplex.at(2) = simplex.at(1);
		simplex.at(1) = simplex.at(0);

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

std::pair<std::pair<float, glm::vec3>, glm::vec3> PhysicsEngine::expandingPolytope(
	std::vector<SupportPoint> &simplex, PhysicsElement &a, PhysicsElement &b) {
	//create a copy of end simplex to be turned into a polytope, it may differ slightly from the original simplex,
	//but the algorithm will work properly regardless - this is just an arbitrary starting point
	std::vector<SupportPoint> polytope;
	for (auto p: simplex) {
		//polytope.push_back(calculateSupportWithPoints(a, b, p));
        polytope.push_back(p);
	}

	std::vector<glm::ivec3> faces = {
		{0, 1, 2},
		{0, 3, 1},
		{0, 2, 3},
		{1, 3, 2}
	};

	auto [normal_list, closest_face] = getFaceNormals(polytope, faces);

	//set up return values
	glm::vec3 min_normal(0, 0, 0);
	float min_distance = INFINITY;

	//run loop until there are no more points beyond the closest face
	while (min_distance == INFINITY) {
		//assign the closes face to the min normal and min distance
		min_normal[0] = normal_list.at(closest_face)[0];
		min_normal[1] = normal_list.at(closest_face)[1];
		min_normal[2] = normal_list.at(closest_face)[2];
		min_distance = normal_list.at(closest_face)[3];

		SupportPoint support_point = calculateSupportWithPoints(a, b, min_normal);
		float next_point_distance = glm::dot(min_normal, support_point.point);

		//Check if the support point in the direction of the closest face is a vertex belonging to that face.
		//This works cause the distance from point a to a plane B is equal to the got product of
		//the normalized normal of plane B and vector ab, where b is any point lying on plane B
		//(this works cause a . b = |a| * |b| * cos(Θ)).
		//
		//We cannot directly compare min_distance == next_point_distance tho due to a very real possibility of a
		//floating point error occurring caused by the support function returning 1 of the faces vertices that isn't
		//the same point we used to calculate min distance (point a). Trigonometry just funky like that.
		//I assume that 0.0001 is a sensible value, under which a floating point error will fly, but not a point that
		//is just close to the plane.
		//
		//This, however, can very much break calculating the depth of collision on very, very small objects. I suppose
		//the dev will just have to keep this in mind. If they use items small enough to break this, they:
		//a) will most likely not notice the errors stemming from this
		//b) will most likely have bigger problems with the physics engine than this (continuous collision detection when?)
		//c) have to be making a game way too ambitious for this engine anyway
		//As such I deem this "good enough", but if you have a better idea please let me know
		if (abs(next_point_distance - min_distance) > 0.0001) {
			//run the loop again
			min_distance = INFINITY;

			//The rest of what we do in here is adding a new support point, and deleting all faces pointing at it.
			//We do this to avoid duplicate faces inside the polytope
			//
			//We begin the process by finding which edges face the same direction and then check for the uniqueness
			//of their edges
			std::vector<std::pair<int, int> > unique_edges;

			for (int i = 0; i < (int) normal_list.size(); i++) {
				//check for direction
				if (glm::dot(glm::vec3(normal_list.at(i)[0], normal_list.at(i)[1], normal_list.at(i)[2]),
				             support_point.point) > 0) {
					//check whether the 3 faces of a given edge are unique and add them to the edge list.
					addUniqueEdge(unique_edges, faces, i, 0, 1);
					addUniqueEdge(unique_edges, faces, i, 1, 2);
					addUniqueEdge(unique_edges, faces, i, 2, 0);

					//delete the face
					faces.at(i) = faces.back();
					faces.pop_back(); //sidenote: I have no idea why .pop_back() does not return the value.
					normal_list.at(i) = normal_list.back();
					normal_list.pop_back();
					//We use pop-erasing and un-iterate i to not screw up the loop
					i--;
				}
			}

			//now that we deleted the non-needed faces, we need to construct new ones that touch the new support point
			std::vector<glm::ivec3> new_faces;
			for (auto [edge, edge2]: unique_edges) {
				new_faces.emplace_back(edge, edge2, polytope.size());
				//this is the part, where I am not sure whether the order remains clockwise
			}

			//add the new support to the polytope
			polytope.push_back(support_point);

			//find normals for the new faces
			auto [new_normal_list, new_closest_face] = getFaceNormals(polytope, new_faces);

			// TODO write an actual fix, avoid crashing for now
//			if (new_normal_list.empty()) {
//				break;
//			}

			//find the new closest face by comparing the closest of the old and new faces
			float old_min_distance = INFINITY;
			for (int i = 0; i < (int) normal_list.size(); i++) {
				if (normal_list.at(i)[3] < old_min_distance) {
					closest_face = i;
					old_min_distance = normal_list.at(i)[3];
				}
			}
			if (new_normal_list.at(new_closest_face)[3] < old_min_distance) {
				closest_face = new_closest_face + normal_list.size();
			}

			//insert the new faces and their normals
			faces.insert(faces.end(), new_faces.begin(), new_faces.end());
			normal_list.insert(normal_list.end(), new_normal_list.begin(), new_normal_list.end());
		}
	}

	//calculating the collision point via barycentric coordinates
	//project the origin onto the closest face to get barycentric coordinates
	glm::ivec3 face = faces[closest_face];
	glm::vec3 p0 = polytope.at(face[0]).point;
	glm::vec3 p1 = polytope.at(face[1]).point;
	glm::vec3 p2 = polytope.at(face[2]).point;
	glm::vec3 normal = glm::cross(p1 - p0, p2 - p0);
	//we can't use the normal from the normal list because it's normalized

	float denominator = glm::dot(normal, normal);
	float weight1 = glm::dot(glm::cross(-p0, p2 - p0), normal) / denominator;
	float weight2 = glm::dot(glm::cross(p1 - p0, -p0), normal) / denominator;
	float weight0 = 1 - weight1 - weight2;

	// Interpolate the support points from A and B
	glm::vec3 collision_a = weight0 * polytope.at(face[0]).point_a + weight1 * polytope.at(face[1]).point_a + weight2 *
	                        polytope.at(face[2]).point_a;
	glm::vec3 collision_b = weight0 * polytope.at(face[0]).point_b + weight1 * polytope.at(face[1]).point_b + weight2 *
	                        polytope.at(face[2]).point_b;

	glm::vec3 collision_point = (collision_a + collision_b) / 2.0f;

	//better to overcompensate the correction distance than under compensate and cause this whole mess to run again
	return {{min_distance * 1.02, min_normal}, collision_point};
}

SupportPoint PhysicsEngine::calculateSupportWithPoints(PhysicsElement &a, PhysicsElement &b, glm::vec3 &direction) {
	glm::vec3 a_point = a.furthestPoint(direction);
	glm::vec3 b_point = b.furthestPoint(-direction);
	return {a_point - b_point, a_point, b_point};
}

std::pair<std::vector<glm::vec4>, int> PhysicsEngine::getFaceNormals(std::vector<SupportPoint> &polytope,
                                                                     std::vector<glm::ivec3> &faces) {
	//prepare variables needed for finding face normals and the closes face
	std::vector<glm::vec4> normal_list;
	int closest_face = 0;
	float min_distance = INFINITY;

	//check normal for each face
	for (int i = 0; i < (int) faces.size(); ++i) {
		//a, b and c are the points making up the face
		glm::vec3 a = polytope.at(faces.at(i)[0]).point;
		glm::vec3 b = polytope.at(faces.at(i)[1]).point;
		glm::vec3 c = polytope.at(faces.at(i)[2]).point;

		//find the normal of a given face from the cross product of vector AB and AC
		glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
		//find distance to the face from the dot product of normal and one of its points
		float distance = glm::dot(normal, a);

		//check if normal is pointing into the polytope
		if (distance < 0) {
			//if so flip the direction of the normal
			normal *= -1;
			distance *= -1;
		}
		//append the normal to the list
		normal_list.emplace_back(normal, distance);

		//check if current face is the closest face
		if (distance < min_distance) {
			closest_face = i;
			min_distance = distance;
		}
	}

	return {normal_list, closest_face};
}

void PhysicsEngine::addUniqueEdge(std::vector<std::pair<int, int> > &edges, std::vector<glm::ivec3> faces, int face_num,
                                  int a, int b) {
	//create edge from 2 vertices given as a and b
	auto edge = std::find(edges.begin(), edges.end(), std::make_pair(faces.at(face_num)[b], faces.at(face_num)[a]));
	auto edge2 = std::find(edges.begin(), edges.end(), std::make_pair(faces.at(face_num)[a], faces.at(face_num)[b]));

	//check if the edge already exists, if it does, it means that the edge would lead to a duplicate face creation, and as such it must be destroyed.
	if (edge != edges.end()) {
										//sidenote: if I implemented everything correctly there would be no need to check both orders, only ba
		edges.erase(edge);			//but I don't trust myself enough to not make absolutely sure this works
	}									//      0--<--3
	else if (edge2 != edges.end())		//     / \ B /   A: 2-0
	{									//    / A \ /    B: 0-2
		edges.erase(edge2);			//   1-->--2
	}									//If everything was clockwise (as above) the same edge for face A would a reverse of it for B

	//if face is unique add it to the unique edges
	else {
		edges.emplace_back(faces.at(face_num)[a], faces.at(face_num)[b]);
	}
}

void PhysicsEngine::applyForces(PhysicsElement &a, PhysicsElement &b, float collision_depth, glm::vec3 &collision_normal, glm::vec3 &collision_point)
{
	//------positional correction------
	glm::vec3 correction_vector = collision_normal * collision_depth;
	a.position = (a.position - correction_vector * (b.mass/(a.mass + b.mass)));
	b.position = (b.position + correction_vector * (a.mass/(a.mass + b.mass)));

    glm::vec3 a_vel = a.velocity;
    glm::vec3 b_vel = b.velocity;

	//-----impulse based collision response-----
	float relative_velocity = glm::dot(b.velocity - a.velocity, collision_normal);
	float inverse_mass_a = 1.0/a.mass;
	float inverse_mass_b = 1.0/b.mass;

	//calculate impulse scalar and vector
	float impulse_scalar = -(1 + a.coefficient_of_restitution/2.0 + b.coefficient_of_restitution/2.0) * relative_velocity / (inverse_mass_a + inverse_mass_b);
	glm::vec3 impulse_vector = impulse_scalar * collision_normal;

    a.velocity -= impulse_vector * inverse_mass_a;
    b.velocity += impulse_vector * inverse_mass_b;

    // -----applying friction-----
    glm::vec3 rel_vel = b_vel - a_vel;

    // project relative velocity onto the collision normal
    glm::vec3 normal_velocity = glm::dot(rel_vel, collision_normal) * collision_normal;

    // tangential velocity = full relative velocity - normal component
    glm::vec3 tangent = rel_vel - normal_velocity;

    // if tangent is near zero, skip friction
    if (glm::length(tangent) > 0.0001f) {
        tangent = glm::normalize(tangent);

        // calculate magnitude of friction impulse
        float friction_scalar = glm::length(impulse_vector); // magnitude of normal impulse

        // average friction coefficients (can be more sophisticated if needed)
        float mu_static = fmin(0.5f * (a.coefficient_of_friction + b.coefficient_of_friction) * 1.2f, 1);
        float mu_dynamic = 0.5f * (a.coefficient_of_friction + b.coefficient_of_friction);

        // calculate tangential impulse magnitude
        float tangential_impulse_magnitude = -glm::dot(rel_vel, tangent) / (inverse_mass_a + inverse_mass_b);

        // clamp friction based on Coulomb's law
        glm::vec3 friction_impulse;
        if (std::abs(tangential_impulse_magnitude) < friction_scalar * mu_static) {
            // static friction
            friction_impulse = tangential_impulse_magnitude * tangent;
        } else {//dynamic friction
        friction_impulse = -friction_scalar * mu_dynamic * tangent;
        }

        // apply friction impulse
        a.velocity -= friction_impulse * inverse_mass_a;
        b.velocity += friction_impulse * inverse_mass_b;
    }
}


double PhysicsEngine::physicsUpdate() {
	//timer start
	auto start = std::chrono::system_clock::now();

	getElements();
	if (elements.size() == 0) return 0.0f ;

	//update all existing objects
	for (auto & element :elements) {
		element.update(TICK_DURATION, gravity_strength);
	}


	//check for collision between every object pair
	for (int i = 0; i < (int) elements.size() - 1; i++) {
		for (int j = i + 1; j < (int) elements.size(); j++) {
			//initial, time efficient, but inaccurate collision detection
			if (initialCollisionCheck(elements[i], elements[j])) {
				//second, more time-consuming, but exact detection
				auto [isColliding, simplex] = gilbertJohnsonKeerthi(elements[i], elements[j]);
				if (isColliding) {
					auto [dn, collision_point] = expandingPolytope(simplex, elements[i], elements[j]);
					auto [collision_depth, collision_normal] = dn;

					//safeguard for objects going deeper into themselves instead of uncolliding
					if (glm::dot(collision_normal, collision_point - elements[i].position) >= glm::dot(
						    collision_normal, collision_point - elements[j].position)) {
						applyForces(elements[i], elements[j], collision_depth, collision_normal, collision_point);
					} else {
						applyForces(elements[j], elements[i], collision_depth, collision_normal, collision_point);
					}

					//TODO on collision function in pawn
				}
			}
		}
	}
	//timer end

    auto objects_to_copy = boardManager->getCurrentBoard().lock()->getTree().getPhysicsComponents();
    int j = 0;
    //something here is really broken TODO
    for (const auto & it : objects_to_copy)
    {
        it.get()->setPosition(elements[j].position);
        it.get()->setVelocity(elements[j].velocity);
        it.get()->setAngularVelocity(elements[j].angular_velocity);
        it.get()->setRotation(elements[j].rotation);
        j++;
    }

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_time = end - start;
	return TICK_DURATION - elapsed_time.count();
}


void PhysicsEngine::getElements() {
	elements.clear();
	if (boardManager->getCurrentBoard().expired()) {
		FAULT("UPS");
		return;
	}
	auto objects_to_copy = boardManager->getCurrentBoard().lock()->getTree().getPhysicsComponents();
	for (auto it = objects_to_copy.begin(); it != objects_to_copy.end(); it++) {
		auto value = it->get();
		PhysicsElement element = {
			value->getPosition(),
			value->getVelocity(),
			value->getRotation(),
			value->getAngularVelocity(),
			value->getCollider().getCenterOfMass(),
			value->getCollider().getVertices(),
			value->getCollider().getTriangles(),
			false,
			value->getGravityScale(),
			value->getCollider().getSphereColliderRadius(),
			value->getMass(),
			value->getCollider().getInertiaTensor(),
			value->getMaterial().coefficient_of_friction,
			value->getMaterial().coefficient_of_restitution
		};
		elements.push_back(element);
        frame_num++;
	}
}

void PhysicsEngine::setGravityScale(const glm::vec3& gravityScale) {
    gravity_strength = gravityScale;
}
