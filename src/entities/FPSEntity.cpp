#include <iostream>
#include "FPSEntity.h"
#include "../core/BaseRunner.h"

using namespace std;
using namespace sf;


FPSEntity::FPSEntity()
    : GameEntity("FPSEntity"),
      updateTime(),
      statsText(nullptr),
      font(nullptr),
      framesPassed(0) {
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
    this->updateFPS(deltaTime);
}

void FPSEntity::draw(RenderWindow* targetWindow) {
    GameEntity::draw(targetWindow);

    if (this->statsText != nullptr)
        targetWindow->draw(*this->statsText);
}

void FPSEntity::updateFPS(Time elapsedTime) {
    this->updateTime += elapsedTime;
    this->framesPassed++;
    
    if (this->updateTime >= seconds(0.125f)) {
        float fps = this->framesPassed / this->updateTime.asSeconds();
        this->statsText->setString("FPS: " + to_string((int)fps) + "\n");
        
        this->updateTime = Time::Zero;
        this->framesPassed = 0;
    }
}
