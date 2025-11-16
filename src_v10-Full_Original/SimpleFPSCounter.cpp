#include "SimpleFPSCounter.h"
#include <iostream>

SimpleFPSCounter::SimpleFPSCounter() : AGameObject("FPSCounter")
{
    font = nullptr;
    fpsText = nullptr;
    framesPassed = 0;
    updateTime = sf::Time::Zero;
}

SimpleFPSCounter::~SimpleFPSCounter()
{
    delete fpsText;
    delete font;
}

void SimpleFPSCounter::initialize()
{
    std::cout << "Initializing FPS Counter" << std::endl;
    
    font = new sf::Font();
    
    #ifdef _WIN32
        if (!font->loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cout << "Warning: Could not load font. FPS counter will not display." << std::endl;
        }
    #elif __APPLE__
        if (!font->loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
            std::cout << "Warning: Could not load font. FPS counter will not display." << std::endl;
        }
    #else // Linux
        if (!font->loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            std::cout << "Warning: Could not load font. FPS counter will not display." << std::endl;
        }
    #endif
    
    fpsText = new sf::Text();
    fpsText->setFont(*font);
    fpsText->setCharacterSize(30);
    fpsText->setFillColor(sf::Color::White);
    fpsText->setPosition(10, 10);
    fpsText->setString("FPS: --");
}

void SimpleFPSCounter::processInput(sf::Event event)
{
    // No input needed
}

void SimpleFPSCounter::update(sf::Time deltaTime)
{
    updateFPS(deltaTime);
}

void SimpleFPSCounter::draw(sf::RenderWindow* targetWindow)
{
    if (fpsText != nullptr)
    {
        targetWindow->draw(*fpsText);
    }
}

void SimpleFPSCounter::updateFPS(sf::Time elapsedTime)
{
    updateTime += elapsedTime;
    framesPassed++;
    
    if (updateTime >= sf::seconds(1.0f))
    {
        float fps = framesPassed / updateTime.asSeconds();
        fpsText->setString("FPS: " + std::to_string((int)fps));
        
        updateTime = sf::Time::Zero;
        framesPassed = 0;
    }
}
