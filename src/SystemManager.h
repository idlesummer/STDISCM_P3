#pragma once
#include <vector>
#include <string>
#include <SFML/System/Time.hpp>
#include "IGameSystem.h"

/**
 * Singleton manager for game systems.
 * Unlike GameObjectManager which manages visual entities,
 * SystemManager handles non-rendering logic systems.
 */
class SystemManager {
public:
    using SystemList = std::vector<IGameSystem*>;

    static SystemManager* getInstance();

    /**
     * Add a system to be managed.
     * SystemManager takes ownership and will delete the system on cleanup.
     */
    void addSystem(IGameSystem* system);

    /**
     * Initialize all systems. Call this once during game startup.
     */
    void initializeAll();

    /**
     * Update all systems. Call this every frame.
     */
    void update(sf::Time deltaTime);

    /**
     * Get the number of active systems.
     */
    int activeSystemCount() const;

private:
    SystemManager();
    ~SystemManager();
    SystemManager(SystemManager const&) = delete;
    SystemManager& operator=(SystemManager const&) = delete;

    static SystemManager* sharedInstance;
    SystemList systems;
};
