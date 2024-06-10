
#include "LOG.hpp"
#include "Physics_Entity.hpp"

Entity::Entity()
{
	Position = sf::Vector2f(0, 0);
	Velocity = sf::Vector2f(0, 20);
	Size = sf::Vector2f(100, 100);
	sprite.setOrigin(50, 50);
	sprite.setFillColor({ 40,40,40 });

	stimuli();
}

Entity::Entity(const sf::Vector2f& position, const sf::Color color)
{
	Position = position;
	Velocity = sf::Vector2f(0, 20);
	Size = sf::Vector2f(70, 70);
	sprite.setFillColor(color);

	stimuli();
}

Entity::~Entity()
{
}

void Entity::stimuli()
{
	sprite.setPosition(Position);
	sprite.setRotation(Rotation);
	sprite.setSize(Size);
	sprite.setOrigin(Size.x / 2, Size.y / 2);

	//auto bounds = sprite.getGlobalBounds();
	//sprite.setOrigin(bounds.width / 2, bounds.height / 2);


	Rigid.PosX = Position.x;
	Rigid.PosY = Position.y;
	Rigid.boundingBox = sprite.getGlobalBounds();
	Rigid.update(0);
}

void Entity::draw(sf::RenderWindow& app)
{
	app.draw(sprite);
	Rigid.draw(app);
}

void Entity::update(double dt)
{
	if (Rigid.BodyType == RigidBody::Active)
	{
		Position.x += Velocity.x * dt;
		Position.y += Velocity.y * dt;
		stimuli();
	}

	Rigid.update(dt);

	Position.x = Rigid.PosX;
	Position.y = Rigid.PosY;
	stimuli();
}
