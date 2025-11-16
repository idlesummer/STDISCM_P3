#pragma once
#include "components/core/AGameObject.h"

class IconObject : public AGameObject {
public:
    IconObject(String name, int textureIndex);
    
    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;

private:
    int textureIndex;
};
