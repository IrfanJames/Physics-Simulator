
#include "LOG.hpp"
#include "Physics.hpp"



namespace Physics {

	std::vector<int> ActiveBodies;
	std::vector<int> RigidBodies;


	void separateActiveBodies()
	{
		ActiveBodies.clear();
		RigidBodies.clear();

		for (int i = 0; i < CircuitGUI::entities.size(); i++)
		{
			CircuitGUI::entities[i].Rigid.isColliding = false;

			if (CircuitGUI::entities[i].Rigid.BodyType == RigidBody::Active)
			{
				ActiveBodies.emplace_back(i);
				RigidBodies.emplace_back(i);
			}
			if (CircuitGUI::entities[i].Rigid.BodyType == RigidBody::Passive)
			{
				RigidBodies.emplace_back(i);
			}
		}
	}

	bool areColliding_AABB(int bodyA, int bodyB, sf::FloatRect& intersection)
	{
		using CircuitGUI::entities;
		
		if (bodyA == bodyB ||
			bodyA >= entities.size() ||
			bodyB >= entities.size())
			return false;
		
		if (entities[bodyA].Rigid.BodyShape == RigidBody::Rectangular ||
			entities[bodyB].Rigid.BodyShape == RigidBody::Rectangular)
		{
			auto& boxA = entities[bodyA].Rigid.boundingBox;
			auto& boxB = entities[bodyB].Rigid.boundingBox;

			return boxA.intersects(boxB, intersection);
		}

		return false;
	}


	void Update(double dt)
	{
		separateActiveBodies();

		//LOG("\n\n")
		for (int a : ActiveBodies)
		{
			auto& active = CircuitGUI::entities[a];

			//LOG("\n")
			for (int r : RigidBodies)
			{
				if (a == r) continue;

				auto& other = CircuitGUI::entities[r];

				sf::FloatRect intersection;
				if (areColliding_AABB(a, r, intersection))
				{
					//LOG("(" << a << ", " << r << ") ; ");
					active.Rigid.isColliding = true;
					other.Rigid.isColliding = true;

					double dx = intersection.width;
					double dy = intersection.height;

					if (other.Rigid.BodyType == RigidBody::Active)
					{
						dx /= 2;
						dy /= 2;

						if (dx < dy)
						{
							if (active.Position.x < intersection.left) {
								active.Position.x -= dx;
								other.Position.x += dx;
							}
							else {
								active.Position.x += dx;
								other.Position.x -= dx;
							}
						}
						else {
							if (active.Position.y < intersection.top) {
								active.Position.y -= dy;
								other.Position.y += dy;
							}
							else {
								active.Position.y += dy;
								other.Position.y -= dy;
							}
						}

						active.stimuli();
						other.stimuli();
					}
					else {
						if (dx < dy) {

							if (active.Position.x < intersection.left)
								active.Position.x -= dx;
							else
								active.Position.x += dx;
						}
						else {

							if (active.Position.y < intersection.top)
								active.Position.y -= dy;
							else
								active.Position.y += dy;
						}

						active.stimuli();
					}

				}
			}
		}
	}
}
