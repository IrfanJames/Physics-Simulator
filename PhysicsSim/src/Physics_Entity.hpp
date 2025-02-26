#pragma once

#include "SFML/Graphics.hpp"
#include "RigidBody.hpp"

class Entity
{
public:
	double Rotation = 0;
	sf::Vector2f Size;
	sf::Vector2f Position;
	sf::Vector2f Velocity;
	sf::RectangleShape sprite;

	RigidBody Rigid;

public:
	Entity();
	Entity(const sf::Vector2f& position, const sf::Color color = sf::Color(40, 40, 40));
	~Entity();

	void stimuli();
	void draw(sf::RenderWindow& app);

	void update(double dt);

private:

};