#include "SimpleCircle.h"
#include <iostream>

SimpleCircle::SimpleCircle(String name, float x, float y, float radius) : 
    AGameObject(name), 
    x(x), 
    y(y), 
    radius(radius)
{
    circle = nullptr;
}

SimpleCircle::~SimpleCircle() {
    delete circle;
}

void SimpleCircle::initialize() {
    std::cout << "Initializing " << getName() << std::endl;
    
    circle = new sf::CircleShape(radius);
    circle->setFillColor(sf::Color::Green);
    circle->setPosition(x, y);
}

void SimpleCircle::processInput(sf::Event event) {
    // No input handling yet
}

void SimpleCircle::update(sf::Time deltaTime) {
    // Move the circle right
    sf::Vector2f pos = circle->getPosition();
    pos.x += speed * deltaTime.asSeconds();
    
    // Wrap around screen
    if (pos.x > 1920)
        pos.x = -radius * 2;
    
    circle->setPosition(pos);
}

void SimpleCircle::draw(sf::RenderWindow* targetWindow) {
    if (circle != nullptr) 
        targetWindow->draw(*circle);
}
