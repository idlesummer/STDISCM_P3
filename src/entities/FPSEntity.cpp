#include <chrono>
#include <iostream>
#include "FPSEntity.h"
#include "../core/BaseRunner.h"

using namespace std;
using namespace sf;


FPSEntity::FPSEntity()
    : GameEntity("FPSEntity"),
      text(nullptr),
      font(nullptr),
      framesPassed(0) {

    // Initialize time points
    startTime = chrono::steady_clock::now();
    lastUpdateTime = startTime;
}

FPSEntity::~FPSEntity() {
    delete this->font;
    delete this->text;
}

void FPSEntity::initialize() {
    this->font = new Font();

    if (!this->font->loadFromFile("assets/Sansation.ttf")) {
        cerr << "[FPSEntity] ERROR: Failed to load font!" << endl;
        return;
    }

    auto WINDOW_WIDTH = static_cast<float>(BaseRunner::WINDOW_WIDTH - 150);
    auto WINDOW_HEIGHT = static_cast<float>(BaseRunner::WINDOW_HEIGHT - 70);

    this->text = new Text();
    this->text->setFont(*this->font);
    this->text->setPosition(WINDOW_WIDTH, WINDOW_HEIGHT);
    this->text->setFillColor(Color::White);
    this->text->setOutlineColor(Color(1, 1, 1));
    this->text->setOutlineThickness(2.5f);
    this->text->setCharacterSize(35);
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
    auto elapsedSeconds = static_cast<float>(elapsed.count()) / 1000.0f;
    auto fps = static_cast<float>(this->framesPassed) / elapsedSeconds;

    this->text->setString("FPS: " + to_string((int)fps) + "\n");

    // Reset counters
    this->lastUpdateTime = currentTime;
    this->framesPassed = 0;
}

void FPSEntity::draw(RenderWindow* targetWindow) {
    if (this->text == nullptr)
        return;

    targetWindow->draw(*this->text);
}
