#pragma once
#include <SFML/System/Time.hpp>

/**
 * Interface for game systems that perform logic but don't render.
 * Unlike AGameObject which represents visual entities with sprites,
 * IGameSystem is for controllers, managers, and coordinators.
 *
 * Examples:
 * - Icon spawning system
 * - AI controller
 * - Physics system
 * - Audio manager
 */
class IGameSystem {
public:
    virtual ~IGameSystem() = default;

    /**
     * Initialize the system. Called once after construction.
     */
    virtual void initialize() = 0;

    /**
     * Update system logic each frame.
     * @param deltaTime Time elapsed since last frame
     */
    virtual void update(sf::Time deltaTime) = 0;

    // Note: No draw() method - systems don't render!
    // If something needs to render, it should create GameObjects instead.
};
