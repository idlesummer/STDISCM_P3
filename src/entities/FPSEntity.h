#pragma once
#include "../types/GameEntity.h"
#include <chrono>

class FPSEntity : public GameEntity {
public:
    FPSEntity();
    ~FPSEntity();

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow* targetWindow) override;

private:
    sf::Text* text;
    sf::Font* font;
    int framesPassed;

    // Use real-time clock for actual FPS measurement
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::chrono::steady_clock::time_point startTime;
};
