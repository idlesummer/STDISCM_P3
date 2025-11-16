#pragma once
#include "AGameObject.h"
#include <vector>

class IconObject;

/// <summary>
/// Manages the progressive loading and display of streaming textures
/// Now uses ThreadPool for parallel loading!
/// Spawns IconObjects in a grid as textures become available
/// </summary>
class TextureDisplay : public AGameObject {
public:
    TextureDisplay();
    ~TextureDisplay();

    void initialize();
    void processInput(sf::Event event);
    void update(sf::Time deltaTime);

private:
    typedef std::vector<IconObject*> IconList;
    IconList iconList;

    const float STREAMING_LOAD_DELAY = 1000.0f;
    float ticks = 0.0f;

    int columnGrid = 0;
    int rowGrid = 0;
    
    const int MAX_COLUMN = 28;
    const int MAX_ROW = 22;
    const int IMG_WIDTH = 68;
    const int IMG_HEIGHT = 68;

    void spawnObject();
};
