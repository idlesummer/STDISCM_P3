#include <iostream>
#include <chrono>
#include "FPSEntity.h"
#include "../core/BaseRunner.h"

using namespace std;
using namespace sf;


FPSEntity::FPSEntity()
    : GameEntity("FPSEntity"),
      statsText(nullptr),
      font(nullptr),
      framesPassed(0) {

    // Initialize time points
    startTime = chrono::steady_clock::now();
    lastUpdateTime = startTime;
}

FPSEntity::~FPSEntity() {
    delete this->font;
    delete this->statsText;
}

void FPSEntity::initialize() {
    this->font = new Font();

    if (!this->font->loadFromFile("assets/Sansation.ttf")) {
        cerr << "[FPSEntity] ERROR: Failed to load font!" << endl;
        return;
    }

    this->statsText = new Text();
    this->statsText->setFont(*this->font);
    this->statsText->setPosition(BaseRunner::WINDOW_WIDTH - 150, BaseRunner::WINDOW_HEIGHT - 70);
    this->statsText->setFillColor(Color::White);
    this->statsText->setOutlineColor(Color(1.0f, 1.0f, 1.0f));
    this->statsText->setOutlineThickness(2.5f);
    this->statsText->setCharacterSize(35);
}

void FPSEntity::processInput(Event event) {
    // no-op
}

void FPSEntity::update(Time deltaTime) {
    this->framesPassed++;

    // Get current time and calculate actual elapsed time
    auto currentTime = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(currentTime - lastUpdateTime);

    // Update every 125ms (same as before, but using real time)
    if (elapsed.count() < 125)
        return;

    // Calculate actual FPS based on real elapsed time
    auto elapsedSeconds = elapsed.count() / 1000.0f;
    auto fps = this->framesPassed / elapsedSeconds;

    this->statsText->setString("FPS: " + to_string((int)fps) + "\n");

    // Reset counters
    this->lastUpdateTime = currentTime;
    this->framesPassed = 0;
}

void FPSEntity::draw(RenderWindow* targetWindow) {
    GameEntity::draw(targetWindow);

    if (this->statsText != nullptr)
        targetWindow->draw(*this->statsText);
}
