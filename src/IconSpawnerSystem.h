#pragma once
#include "GameSystem.h"
#include <vector>
#include <SFML/System/Time.hpp>

class IconEntity;

/**
 * System responsible for spawning and managing icon entities.
 * This is a controller/system, not a visual game entity.
 *
 * Responsibilities:
 * - Coordinate texture loading batches with TextureManager
 * - Spawn IconEntity instances when textures are ready
 * - Manage grid layout positioning for icons
 * - Track spawned icons
 */
class IconSpawnerSystem : public GameSystem {
public:
    IconSpawnerSystem();
    ~IconSpawnerSystem();

    void initialize() override;
    void update(sf::Time deltaTime) override;

private:
    using IconList = std::vector<IconEntity*>;

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
