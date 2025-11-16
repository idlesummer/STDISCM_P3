#pragma once
#include "../types/GameEntity.h"

class BGEntity : public GameEntity {
public:
    BGEntity(String name);

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;

private:
    float SPEED_MULTIPLIER;
};
