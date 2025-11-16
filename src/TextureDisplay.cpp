#include <iostream>
#include "TextureDisplay.h"
#include "IconObject.h"
#include "TextureManager.h"
#include "GameObjectManager.h"
#include "BaseRunner.h"

using namespace std;
using namespace sf;


TextureDisplay::TextureDisplay() 
    : AGameObject("TextureDisplay"),
      iconList(),
      STREAMING_LOAD_DELAY(1000.0f),
      ticks(0.0f),
      columnGrid(0),
      rowGrid(0),
      MAX_COLUMN(28),
      MAX_ROW(22),
      IMG_WIDTH(68),
      IMG_HEIGHT(68) {

    // no-op
}

TextureDisplay::~TextureDisplay() {
    // no-op
}

void TextureDisplay::initialize() {
    cout << "TextureDisplay initialized (Full Original Mode)" << endl;
    cout << "Grid: " << this->MAX_COLUMN << " columns × " << this->MAX_ROW << " rows" << endl;
    cout << "Icon size: " << this->IMG_WIDTH << "×" << this->IMG_HEIGHT << " pixels" << endl;
    cout << "Textures will load in PARALLEL using thread pool!" << endl;
    cout << "Loading ALL 480 streaming assets!" << endl;
}

void TextureDisplay::processInput(Event event) {
    // no-op
}

void TextureDisplay::update(Time deltaTime) {
    auto* textureManager = TextureManager::getInstance();
    this->ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();
    
    if (this->ticks >= this->STREAMING_LOAD_DELAY) {
        this->ticks = 0.0f;
        
        int currentIndex = this->iconList.size();
        int batchSize = 20;
        const int MAX_ICONS = 480;
        
        if (currentIndex < MAX_ICONS) {
            cout << "[TextureDisplay] Scheduling batch load starting at index " << currentIndex << endl;
            textureManager->loadBatchAsync(currentIndex, batchSize);
        }
    }
    
    while (textureManager->hasReadyTexture()) {
        auto loaded = textureManager->popReadyTexture();
        
        cout << "[TextureDisplay] Spawning icon for texture " << loaded.index << endl;
        this->spawnObject();
        
        cout << "[TextureDisplay] Icons spawned: " << this->iconList.size() 
             << "/480 (Ready queue: " 
             << textureManager->getReadyQueueSize() << ")" << endl;
    }
}

void TextureDisplay::spawnObject() {
    auto name = "Icon_" + to_string(this->iconList.size());
    auto icon = new IconObject(name, this->iconList.size());
    this->iconList.push_back(icon);
    
    float x = this->columnGrid * this->IMG_WIDTH;
    float y = this->rowGrid    * this->IMG_HEIGHT;
    icon->setPosition(x, y);
    
    cout << "Spawned " << name << " at position (" << x << ", " << y << ")" << endl;
    
    this->columnGrid++;
    
    if (this->columnGrid >= this->MAX_COLUMN) {
        this->columnGrid = 0;
        this->rowGrid++;
    }
    
    GameObjectManager::getInstance()->addObject(icon);
}
