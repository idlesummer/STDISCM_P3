#include "components/ui/IconObject.h"
#include "lib/managers/TextureManager.h"
#include <iostream>

using namespace std;
using namespace sf;


IconObject::IconObject(String name, int textureIndex) 
    : AGameObject(name),
      textureIndex(textureIndex) {
}

void IconObject::initialize() {
    auto texture = TextureManager::getInstance()->getStreamTextureFromList(this->textureIndex);
    this->sprite = new Sprite();
    this->sprite->setTexture(*texture);
}

void IconObject::processInput(Event event) {
    // no-op
}

void IconObject::update(Time deltaTime) {
    // no-op
}
