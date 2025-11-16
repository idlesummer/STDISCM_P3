#pragma once
#include "AGameObject.h"
#include <vector>

class IconObject;

/// <summary>
/// Manages the progressive loading and display of streaming textures
/// Spawns IconObjects in a grid as textures become available
/// </summary>
class TextureDisplay : public AGameObject
{
public:
    TextureDisplay();
    ~TextureDisplay();

    void initialize();
    void processInput(sf::Event event);
    void update(sf::Time deltaTime);

private:
    typedef std::vector<IconObject*> IconList;
    IconList iconList;

    const float STREAMING_LOAD_DELAY = 1000.0f;  // Milliseconds between loads
    float ticks = 0.0f;

    // Grid layout
    int columnGrid = 0;
    int rowGrid = 0;
    
    const int MAX_COLUMN = 28;  // Icons per row
    const int MAX_ROW = 22;     // Max rows
    const int IMG_WIDTH = 68;   // Icon width in pixels
    const int IMG_HEIGHT = 68;  // Icon height in pixels

    void spawnObject();
};
