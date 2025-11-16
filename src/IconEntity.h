#pragma once
#include "GameEntity.h"

class IconEntity : public GameEntity {
public:
    IconEntity(String name, int textureIndex);

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;

private:
    int textureIndex;
};
