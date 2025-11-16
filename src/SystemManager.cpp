#include "SystemManager.h"
#include <iostream>

using namespace std;

SystemManager* SystemManager::sharedInstance = nullptr;

SystemManager::SystemManager() : systems() {
    cout << "SystemManager created" << endl;
}

SystemManager::~SystemManager() {
    // Clean up all systems
    for (auto* system : this->systems) {
        delete system;
    }
    this->systems.clear();
}

SystemManager* SystemManager::getInstance() {
    if (sharedInstance == nullptr) {
        sharedInstance = new SystemManager();
    }
    return sharedInstance;
}

void SystemManager::addSystem(GameSystem* system) {
    if (system != nullptr) {
        this->systems.push_back(system);
        cout << "System added to SystemManager (total: "
             << this->systems.size() << ")" << endl;
    }
}

void SystemManager::initializeAll() {
    cout << "Initializing " << this->systems.size() << " system(s)..." << endl;
    for (auto* system : this->systems) {
        system->initialize();
    }
    cout << "All systems initialized" << endl;
}

void SystemManager::update(sf::Time deltaTime) {
    for (auto* system : this->systems) {
        system->update(deltaTime);
    }
}

int SystemManager::activeSystemCount() const {
    return this->systems.size();
}
