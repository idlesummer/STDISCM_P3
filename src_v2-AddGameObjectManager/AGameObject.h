#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class AGameObject : sf::NonCopyable
{
public:
    typedef std::string String;
    
    AGameObject(String name);
    virtual ~AGameObject();

    virtual void initialize() = 0;
    virtual void processInput(sf::Event event) = 0;
    virtual void update(sf::Time deltaTime) = 0;
    virtual void draw(sf::RenderWindow* targetWindow);
    
    String getName();

protected:
    String name;
};
