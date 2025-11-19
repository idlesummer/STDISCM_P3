#pragma once
#include "../core/Entity.hpp"
#include "../core/AssetManager.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

using namespace std;
using namespace sf;

class IconScrollDisplay : public Entity {
private:
    static constexpr float CELL_SIZE = 30.0f;
    static constexpr int GRID_WIDTH = 10;
    static constexpr int GRID_HEIGHT = 20;
    static constexpr float SCROLL_INTERVAL = 0.5f; // Half a second

    RectangleShape cellShape;
    Vector2f displayPosition;

    vector<string> textureNames;
    vector<vector<int>> grid; // Stores texture indices

    Time scrollTimer;
    int currentTextureIndex;
    bool isActive;

public:
    IconScrollDisplay(Vector2f position)
        : Entity("IconScrollDisplay", position),
          cellShape(),
          displayPosition(position),
          grid(GRID_HEIGHT, vector<int>(GRID_WIDTH, -1)),
          scrollTimer(Time::Zero),
          currentTextureIndex(0),
          isActive(false) {
    }

    void onCreate() override {
        // Setup cell shape for rendering
        this->cellShape.setSize(Vector2f(CELL_SIZE, CELL_SIZE));
        this->cellShape.setOutlineThickness(1.0f);
        this->cellShape.setOutlineColor(Color(100, 100, 100));
        this->cellShape.setFillColor(Color::White);
    }

    void start() {
        this->isActive = true;
        this->currentTextureIndex = 0;
        this->scrollTimer = Time::Zero;

        // Get all loaded texture names (do this here, after loading is complete)
        auto& assetManager = AssetManager::getInstance();
        this->textureNames = assetManager.getTextureNames();

        // Clear grid
        for (auto& row : this->grid) {
            row.assign(GRID_WIDTH, -1);
        }

        // Fill first row with icons
        this->fillTopRow();
    }

    void stop() {
        this->isActive = false;
    }

    auto isDisplayActive() const -> bool {
        return this->isActive;
    }

    void onUpdate(Time deltaTime) override {
        if (!this->isActive)
            return;

        this->scrollTimer += deltaTime;

        // Scroll every half second
        if (this->scrollTimer.asSeconds() >= SCROLL_INTERVAL) {
            this->scrollTimer = Time::Zero;
            this->scrollDown();
        }
    }

    void onDraw(RenderWindow& window) override {
        if (!this->isActive)
            return;

        auto& assetManager = AssetManager::getInstance();

        // Draw grid with icons
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                int textureIdx = this->grid[y][x];
                if (textureIdx < 0)
                    continue;

                auto posX = this->displayPosition.x + x * CELL_SIZE;
                auto posY = this->displayPosition.y + y * CELL_SIZE;

                this->cellShape.setPosition(posX, posY);

                // Get texture and draw it (no color tinting - just white/grayscale)
                if (!this->textureNames.empty()) {
                    auto textureName = this->textureNames[textureIdx % this->textureNames.size()];
                    auto texture = assetManager.getTexture(textureName);

                    if (texture) {
                        this->cellShape.setTexture(texture.get());
                        window.draw(this->cellShape);
                    }
                }
            }
        }
    }

private:
    void scrollDown() {
        // Move all rows down by 1
        for (int y = GRID_HEIGHT - 1; y > 0; y--) {
            this->grid[y] = this->grid[y - 1];
        }

        // Clear top row
        this->grid[0].assign(GRID_WIDTH, -1);

        // Fill top row with next icons
        this->fillTopRow();
    }

    void fillTopRow() {
        if (this->textureNames.empty())
            return;

        for (int x = 0; x < GRID_WIDTH; x++) {
            this->grid[0][x] = this->currentTextureIndex;
            this->currentTextureIndex++;

            // Loop back to beginning when we've shown all textures
            if (this->currentTextureIndex >= static_cast<int>(this->textureNames.size())) {
                this->currentTextureIndex = 0;
            }
        }
    }
};
