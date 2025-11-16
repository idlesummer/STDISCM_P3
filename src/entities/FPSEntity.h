#pragma once
#include "../types/GameEntity.h"

class FPSEntity : public GameEntity {
public:
    FPSEntity();
    ~FPSEntity();
    
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
