
#include "RigidBody.hpp"


RigidBody::RigidBody()
{
	boundingBoxDisplay.setFillColor(sf::Color::Transparent);
	boundingBoxDisplay.setOutlineThickness(1.0f);
	boundingBoxDisplay.setOutlineColor(sf::Color::Green);

	boundingCircleDisplay.setFillColor(sf::Color::Transparent);
	boundingCircleDisplay.setOutlineThickness(1.0f);
	boundingCircleDisplay.setOutlineColor(sf::Color::Green);

	update(0);
}

RigidBody::~RigidBody()
{
}

void RigidBody::updateBoundDisplay()
{
	boundingBox.left = PosX - boundingBox.width / 2;
	boundingBox.top = PosY - boundingBox.height / 2;

	switch (BodyShape)
	{
	case Shape::Rectangular: {
		boundingBoxDisplay.setPosition(boundingBox.left, boundingBox.top);
		boundingBoxDisplay.setSize(sf::Vector2f(boundingBox.width, boundingBox.height));

		if (isColliding) boundingBoxDisplay.setOutlineColor(sf::Color::Red);
		else			 boundingBoxDisplay.setOutlineColor(sf::Color::Green);

		break;
	}
	case Shape::Circular: {

		boundingCircleDisplay.setPointCount(50);
	}
	case Shape::ConvexShape: {
		float radius = std::max(boundingBox.width, boundingBox.height) * 0.55;

		boundingCircleDisplay.setRadius(radius);
		boundingCircleDisplay.setOrigin(radius, radius);
		boundingCircleDisplay.setPosition(boundingBox.left + boundingBox.width / 2, boundingBox.top + boundingBox.height / 2);

		boundingCircleDisplay.setPointCount(7);

		if (isColliding) boundingCircleDisplay.setOutlineColor(sf::Color::Red);
		else			 boundingCircleDisplay.setOutlineColor(sf::Color::Green);
	}
	default: break;
	}
}

void RigidBody::draw(sf::RenderWindow& app)
{
	if(BodyType != RigidBodyType::NotRigid)
	{
		if (BodyShape == Shape::Rectangular)
			app.draw(boundingBoxDisplay);
		else
			app.draw(boundingCircleDisplay);
	}
}


void RigidBody::update(double dt)
{

	

	updateBoundDisplay();
}