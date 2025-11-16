#pragma once
#include "AGameObject.h"

class SimpleFPSCounter : public AGameObject
{
public:
    SimpleFPSCounter();
    ~SimpleFPSCounter();
    
    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow* targetWindow) override;

private:
    sf::Text* fpsText;
    sf::Font* font;
    
    sf::Time updateTime;
    int framesPassed;
    
    void updateFPS(sf::Time elapsedTime);
};
