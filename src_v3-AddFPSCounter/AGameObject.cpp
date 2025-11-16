#include "AGameObject.h"

AGameObject::AGameObject(String name) {
    this->name = name;
}

AGameObject::~AGameObject() {
}

AGameObject::String AGameObject::getName() {
    return this->name;
}

void AGameObject::draw(sf::RenderWindow* targetWindow) {
    // Base implementation does nothing
    // Derived classes will override this
}
