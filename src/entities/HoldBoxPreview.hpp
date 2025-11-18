#pragma once
#include "../core/Entity.hpp"
#include "../utils/TetrominoShapes.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;
using namespace Tetris;

class HoldBoxPreview : public Entity {
private:
    char heldType;
    ShapeMatrix heldShape;
    Color heldColor;
    RectangleShape blockShape;
    RectangleShape border;
    Text label;
    Font font;
    bool isLocked; // Visual feedback when hold is locked

public:
    HoldBoxPreview(Vector2f position)
        : Entity("HoldBoxPreview", position),
          heldType('\0'),
          heldShape(),
          heldColor(),
          blockShape(),
          border(),
          label(),
          font(),
          isLocked(false) {
    }

    void onCreate() override {
        // Setup label
        this->font.loadFromFile("assets/fonts/sansation.ttf");
        this->label.setFont(this->font);
        this->label.setString("Hold:");
        this->label.setCharacterSize(20);
        this->label.setFillColor(Color::White);
        this->label.setPosition(this->position);

        // Setup border for preview area
        this->border.setSize(Vector2f(120, 120));
        this->border.setPosition(this->position.x, this->position.y + 30);
        this->border.setFillColor(Color::Transparent);
        this->border.setOutlineThickness(2.0f);
        this->border.setOutlineColor(Color::White);

        // Setup block shape
        this->blockShape.setSize(Vector2f(BLOCK_SIZE - 1.0f, BLOCK_SIZE - 1.0f));
        this->blockShape.setOutlineThickness(1.0f);
        this->blockShape.setOutlineColor(Color(50, 50, 50));
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->label);
        window.draw(this->border);

        if (this->heldType == '\0')
            return;

        // Calculate bounding box of the piece
        int minX = 4, maxX = -1;
        int minY = 4, maxY = -1;

        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (this->heldShape[y][x] != 0) {
                    minX = min(minX, x);
                    maxX = max(maxX, x);
                    minY = min(minY, y);
                    maxY = max(maxY, y);
                }
            }
        }

        // Calculate the center of the preview box
        float borderX = this->position.x;
        float borderY = this->position.y + 30;
        float borderWidth = 120;
        float borderHeight = 120;
        float centerX = borderX + borderWidth / 2.0f;
        float centerY = borderY + borderHeight / 2.0f;

        // Calculate the center of the piece in grid coordinates
        float pieceCenterX = (minX + maxX + 1) / 2.0f;
        float pieceCenterY = (minY + maxY + 1) / 2.0f;

        // Calculate offset to center the piece in the preview box
        float offsetX = centerX - pieceCenterX * BLOCK_SIZE;
        float offsetY = centerY - pieceCenterY * BLOCK_SIZE;

        // Draw the held piece centered
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (this->heldShape[y][x] == 0)
                    continue;

                float posX = offsetX + x * BLOCK_SIZE;
                float posY = offsetY + y * BLOCK_SIZE;
                this->blockShape.setPosition(posX, posY);

                // Grey out the piece if hold is locked
                Color displayColor = this->isLocked
                    ? Color(100, 100, 100, 150)
                    : this->heldColor;

                this->blockShape.setFillColor(displayColor);
                window.draw(this->blockShape);
            }
        }
    }

    void setHeldPiece(char type) {
        this->heldType = type;
        if (type != '\0') {
            this->heldShape = getBaseShape(type);
            this->heldColor = getTetrominoColor(type);
        }
    }

    void setLocked(bool locked) {
        this->isLocked = locked;
    }

    auto getHeldType() const { return this->heldType; }
    auto isHoldLocked() const { return this->isLocked; }
};
