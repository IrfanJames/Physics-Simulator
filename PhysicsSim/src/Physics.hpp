#pragma once

#include <vector>
#include "RigidBody.hpp"
#include "Physics_GUI.hpp"

namespace Physics {

	extern std::vector<int> ActiveBodies;
	extern std::vector<int> RigidBodies;

	void separateActiveBodies();

	bool areColliding_AABB(int bodyA, int bodyB, sf::FloatRect& intersection);

	void Update(double dt);
}