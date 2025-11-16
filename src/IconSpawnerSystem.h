#pragma once
#include "IGameSystem.h"
#include <vector>
#include <SFML/System/Time.hpp>

class IconObject;

/**
 * System responsible for spawning and managing icon objects.
 * This is a controller/system, not a visual game object.
 *
 * Responsibilities:
 * - Coordinate texture loading batches with TextureManager
 * - Spawn IconObject instances when textures are ready
 * - Manage grid layout positioning for icons
 * - Track spawned icons
 */
class IconSpawnerSystem : public IGameSystem {
public:
    IconSpawnerSystem();
    ~IconSpawnerSystem();

    void initialize() override;
    void update(sf::Time deltaTime) override;

private:
    using IconList = std::vector<IconObject*>;

    IconList spawnedIcons;

    // Timing for batch loading
    float streamingLoadDelay;
    float timer;
    int batchSize;

    // Grid layout configuration
    int columnGrid;
    int rowGrid;
    int maxColumns;
    int maxRows;
    int iconWidth;
    int iconHeight;

    // Helper methods
    void spawnNextIcon();
    void requestNextBatch();
    void processReadyTextures();
};
