#include "IconEntity.h"
#include "../managers/TextureManager.h"
#include <iostream>

using namespace std;
using namespace sf;


IconEntity::IconEntity(String name, int textureIndex)
    : GameEntity(name),
      textureIndex(textureIndex) {
}

void IconEntity::initialize() {
    auto texture = TextureManager::getInstance()->getStreamTextureFromList(this->textureIndex);
    this->sprite = new Sprite();
    this->sprite->setTexture(*texture);
}

void IconEntity::processInput(Event event) {
    // no-op
}

void IconEntity::update(Time deltaTime) {
    // no-op
}
