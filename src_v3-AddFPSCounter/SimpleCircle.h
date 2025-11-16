#pragma once
#include "AGameObject.h"

class SimpleCircle : public AGameObject
{
public:
    SimpleCircle(String name, float x, float y, float radius);
    ~SimpleCircle();
    
    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow* targetWindow) override;

private:
    sf::CircleShape* circle;
    float x, y, radius;
    float speed = 100.0f;
};
