#include "IconSpawnerSystem.h"
#include "IconEntity.h"
#include "TextureManager.h"
#include "EntityManager.h"
#include "BaseRunner.h"
#include <iostream>

using namespace std;

IconSpawnerSystem::IconSpawnerSystem()
    : spawnedIcons(),
      streamingLoadDelay(1000.0f),
      timer(0.0f),
      batchSize(20),
      columnGrid(0),
      rowGrid(0),
      maxColumns(28),
      maxRows(22),
      iconWidth(68),
      iconHeight(68) {
}

IconSpawnerSystem::~IconSpawnerSystem() {
    // IconEntities are managed by EntityManager, so we don't delete them here
    // We just clear our reference list
    this->spawnedIcons.clear();
}

void IconSpawnerSystem::initialize() {
    cout << "=== IconSpawnerSystem initialized ===" << endl;
    cout << "Grid: " << this->maxColumns << " columns × " << this->maxRows << " rows" << endl;
    cout << "Icon size: " << this->iconWidth << "×" << this->iconHeight << " pixels" << endl;
    cout << "Batch size: " << this->batchSize << " textures" << endl;
    cout << "Load delay: " << this->streamingLoadDelay << "ms" << endl;
    cout << "Total icons to spawn: 480" << endl;
    cout << "Textures will load in PARALLEL using thread pool!" << endl;
}

void IconSpawnerSystem::update(sf::Time deltaTime) {
    this->requestNextBatch();
    this->processReadyTextures();
}

void IconSpawnerSystem::requestNextBatch() {
    auto* textureManager = TextureManager::getInstance();
    this->timer += BaseRunner::TIME_PER_FRAME.asMilliseconds();

    if (this->timer >= this->streamingLoadDelay) {
        this->timer = 0.0f;

        int currentIndex = this->spawnedIcons.size();
        const int MAX_ICONS = 480;

        if (currentIndex < MAX_ICONS) {
            cout << "[IconSpawnerSystem] Scheduling batch load starting at index "
                 << currentIndex << endl;
            textureManager->loadBatchAsync(currentIndex, this->batchSize);
        }
    }
}

void IconSpawnerSystem::processReadyTextures() {
    auto* textureManager = TextureManager::getInstance();

    while (textureManager->hasReadyTexture()) {
        auto loaded = textureManager->popReadyTexture();

        cout << "[IconSpawnerSystem] Spawning icon for texture " << loaded.index << endl;
        this->spawnNextIcon();

        cout << "[IconSpawnerSystem] Icons spawned: " << this->spawnedIcons.size()
             << "/480 (Ready queue: "
             << textureManager->getReadyQueueSize() << ")" << endl;
    }
}

void IconSpawnerSystem::spawnNextIcon() {
    int iconIndex = this->spawnedIcons.size();
    auto name = "Icon_" + to_string(iconIndex);

    // Create the icon entity
    auto* icon = new IconEntity(name, iconIndex);
    this->spawnedIcons.push_back(icon);

    // Calculate grid position
    float x = this->columnGrid * this->iconWidth;
    float y = this->rowGrid * this->iconHeight;
    icon->setPosition(x, y);

    cout << "Spawned " << name << " at position (" << x << ", " << y << ")" << endl;

    // Update grid coordinates
    this->columnGrid++;
    if (this->columnGrid >= this->maxColumns) {
        this->columnGrid = 0;
        this->rowGrid++;
    }

    // Add to EntityManager for rendering
    EntityManager::getInstance()->addEntity(icon);
}
