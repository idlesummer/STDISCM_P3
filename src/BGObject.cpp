#include <iostream>
#include "BGObject.h"
#include "TextureManager.h"
#include "BaseRunner.h"

using namespace std;
using namespace sf;


BGObject::BGObject(string name) 
    : AGameObject(name),
      SPEED_MULTIPLIER(100.0f) {
}

void BGObject::initialize() {
    cout << "Declared as " << this->getName() << endl;
    
    this->sprite = new Sprite();
    auto texture = TextureManager::getInstance()->getFromTextureMap("Desert", 0);
    texture->setRepeated(true);
    this->sprite->setTexture(*texture);
    
    this->sprite->setTextureRect(IntRect(0, 0, BaseRunner::WINDOW_WIDTH, BaseRunner::WINDOW_HEIGHT * 8));
    this->setPosition(0, -BaseRunner::WINDOW_HEIGHT * 7);
}

void BGObject::processInput(Event event) {
    // no-op
}

void BGObject::update(Time deltaTime) {
    auto position = this->getPosition();
    position.y += this->SPEED_MULTIPLIER * deltaTime.asSeconds();
    this->setPosition(position.x, position.y);
    
    auto localPos = this->sprite->getPosition();
    
    if (localPos.y * deltaTime.asSeconds() > 0)
        this->setPosition(0, -BaseRunner::WINDOW_HEIGHT * 7);
}
