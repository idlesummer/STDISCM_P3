#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class AGameObject : sf::NonCopyable {
public:
    using String = std::string;
    
    AGameObject(String name);
    virtual ~AGameObject();

    virtual void initialize() = 0;
    virtual void processInput(sf::Event event) = 0;
    virtual void update(sf::Time deltaTime) = 0;
    virtual void draw(sf::RenderWindow* targetWindow);
    
    String getName();
    void setPosition(float x, float y);
    void setScale(float x, float y);
    sf::FloatRect getLocalBounds();
    sf::Vector2f getPosition();
    sf::Vector2f getScale();

protected:
    String name;
    sf::Sprite* sprite;
    sf::Texture* texture;
    float posX;
    float posY;
    float scaleX;
    float scaleY;
};
