#pragma once
#include "AGameObject.h"
#include <vector>

class IconObject;

class TextureDisplay : public AGameObject {
public:
    TextureDisplay();
    ~TextureDisplay();
    
    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;

private:
    using IconList = std::vector<IconObject*>;
    
    IconList iconList;
    float STREAMING_LOAD_DELAY;
    float ticks;
    int columnGrid;
    int rowGrid;
    int MAX_COLUMN;
    int MAX_ROW;
    int IMG_WIDTH;
    int IMG_HEIGHT;
    
    void spawnObject();
};
