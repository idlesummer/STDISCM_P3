#include "BaseRunner.h"
#include <iostream>

const sf::Time BaseRunner::TIME_PER_FRAME = sf::seconds(1.f / 60.f);

BaseRunner::BaseRunner() :
    window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),     // Creates a 800×600 window
        "Minimal v1: Just a Window",                    // titled "Minimal v1: Just a Window"
        sf::Style::Close                                // Sets it to only have a close button
    )
{
    std::cout << "Window created! Press ESC or close button to exit." << std::endl;
}

void BaseRunner::run() {
    sf::Clock clock;                                    // Clock immediately starts ticking
    sf::Time timeSinceLastUpdate = sf::Time::Zero;      // i.e. accumulatedTime

    while (window.isOpen()) {
        sf::Time elapsedTime = clock.restart();         // First call returns time since clock creation

        timeSinceLastUpdate += elapsedTime;             // This could just be simplified into
        while (timeSinceLastUpdate > TIME_PER_FRAME) {  // for each tick in convertToTicks(time).
            timeSinceLastUpdate -= TIME_PER_FRAME;      // This simulates a division operation

            processEvents();
            update(TIME_PER_FRAME);
        }

        render();
    }
    /*
    Better example of the while loop above:

    while window.isOpen():                          // Main game loop

        accumulatedTime += clock.restart()          // Collect time since last frame
        ticks = accumulatedTime // TIME_PER_FRAME   // Convert time into ticks
        accumulatedTime %= TIME_PER_FRAME           // Keep remaining time not counted as ticks

        for tick in range(ticks):                   // Update game state for every tick
            processEvents()                         
            update(TIME_PER_FRAME)

        render()                                    // Display current state 
                                                    // 1 main game tick = 1 display tick
     */
}

void BaseRunner::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void BaseRunner::update(sf::Time elapsedTime) {
    // Nothing to update yet!
}

void BaseRunner::render() {
    window.clear(sf::Color::Black);
    // Nothing to draw yet!
    window.display();
}
