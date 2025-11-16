#pragma once
#include "GameEntity.h"

class FPSCounter : public GameEntity {
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
