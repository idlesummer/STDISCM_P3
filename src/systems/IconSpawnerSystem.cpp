#include "IconSpawnerSystem.h"
#include "../entities/IconEntity.h"
#include "../managers/TextureManager.h"
#include "../managers/EntityManager.h"
#include "../core/BaseRunner.h"
#include <iostream>

using namespace std;

IconSpawnerSystem::IconSpawnerSystem()
    : spawnedIcons(),
      streamingLoadDelay(1000.0f),
      timer(0.0f),
      batchSize(5),
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
    // Early exit if we've already spawned all icons
    const int MAX_ICONS = 480;
    int iconIndex = static_cast<int>(this->spawnedIcons.size());
    if (iconIndex >= MAX_ICONS)
        return;

    // Timer management
    this->timer += static_cast<float>(BaseRunner::TIME_PER_FRAME.asMilliseconds());
    if (this->timer < this->streamingLoadDelay)
        return;

    auto* textureManager = TextureManager::getInstance();
    this->timer = 0.0f;

    textureManager->loadBatchAsync(iconIndex, this->batchSize);
}

void IconSpawnerSystem::processReadyTextures() {
    auto* textureManager = TextureManager::getInstance();

    while (textureManager->hasReadyTexture()) {
        auto loaded = textureManager->popReadyTexture();
        this->spawnNextIcon();
    }
}

void IconSpawnerSystem::spawnNextIcon() {
    int iconIndex = static_cast<int>(this->spawnedIcons.size());
    auto name = "Icon_" + to_string(iconIndex);

    // Create the icon entity
    auto* icon = new IconEntity(name, iconIndex);
    this->spawnedIcons.push_back(icon);

    // Calculate grid position
    float x = static_cast<float>(this->columnGrid * this->iconWidth);
    float y = static_cast<float>(this->rowGrid * this->iconHeight);
    icon->setPosition(x, y);

    // Update grid coordinates
    this->columnGrid++;
    if (this->columnGrid >= this->maxColumns) {
        this->columnGrid = 0;
        this->rowGrid++;
    }

    // Add to EntityManager for rendering
    EntityManager::getInstance()->addEntity(icon);
}
