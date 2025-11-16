#pragma once
#include "AGameObject.h"

/// <summary>
/// Displays a single icon texture from the streaming texture list
/// Used to create a grid of icons that appear as textures load
/// </summary>
class IconObject : public AGameObject
{
public:
    IconObject(String name, int textureIndex);
    void initialize();
    void processInput(sf::Event event);
    void update(sf::Time deltaTime);

private:
    int textureIndex = 0;  // Index in streamTextureList
};
