#pragma once
#include <SFML/Graphics.hpp>

class BaseRunner : private sf::NonCopyable {
public:
    static const sf::Time TIME_PER_FRAME;
    static const int WINDOW_WIDTH;
    static const int WINDOW_HEIGHT;
    
    BaseRunner();
    void run();

private:
    sf::RenderWindow window;
    
    void render();
    void processEvents();
    void update(sf::Time elapsedTime);
};
