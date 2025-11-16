#pragma once
#include "AGameObject.h"

class FPSCounter : public AGameObject {
public:
    FPSCounter();
    ~FPSCounter();
    
    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow* targetWindow) override;

private:
    sf::Time updateTime;
    sf::Text* statsText;
    sf::Font* font;
    int framesPassed;
    
    void updateFPS(sf::Time elapsedTime);
};
