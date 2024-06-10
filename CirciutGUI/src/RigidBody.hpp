#pragma once

#include "SFML/Graphics.hpp"

class RigidBody
{
public:
	enum RigidBodyType {
		NotRigid = 0,
		Passive,
		Active
	};
	enum Shape {
		Circular = 0,
		Rectangular,
		ConvexShape
	};

	Shape BodyShape = Shape::Rectangular;
	RigidBodyType BodyType = RigidBodyType::Passive;

	bool isColliding = false;
	double PosX, PosY;
	sf::FloatRect boundingBox;

public:
	RigidBody();
	~RigidBody();

	void draw(sf::RenderWindow& app);

	void update(double dt);

private:
	sf::RectangleShape boundingBoxDisplay;
	sf::CircleShape boundingCircleDisplay;

	void updateBoundDisplay();
};

