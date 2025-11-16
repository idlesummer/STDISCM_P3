#include "GameEntity.h"

using namespace std;
using namespace sf;


GameEntity::GameEntity(String name)
    : name(name),
      sprite(nullptr),
      texture(nullptr),
      posX(0.0f),
      posY(0.0f),
      scaleX(1.0f),
      scaleY(1.0f) {
}

GameEntity::~GameEntity() {
    delete this->sprite;
    delete this->texture;
}

GameEntity::String GameEntity::getName() {
    return this->name;
}

void GameEntity::draw(RenderWindow* targetWindow) {
    if (this->sprite == nullptr)
        return;

    this->sprite->setPosition(this->posX, this->posY);
    this->sprite->setScale(this->scaleX, this->scaleY);
    targetWindow->draw(*this->sprite);
}

void GameEntity::setPosition(float x, float y) {
    this->posX = x;
    this->posY = y;

    if (this->sprite != nullptr)
        this->sprite->setPosition(this->posX, this->posY);
}

void GameEntity::setScale(float x, float y) {
    this->scaleX = x;
    this->scaleY = y;

    if (this->sprite != nullptr)
        this->sprite->setScale(this->scaleX, this->scaleY);
}

Vector2f GameEntity::getPosition() {
    return this->sprite->getPosition();
}

Vector2f GameEntity::getScale() {
    return this->sprite->getScale();
}

FloatRect GameEntity::getLocalBounds() {
    return this->sprite->getLocalBounds();
}
