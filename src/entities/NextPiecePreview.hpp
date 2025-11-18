#pragma once
#include "../core/Entity.hpp"
#include "../utils/TetrominoShapes.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;
using namespace Tetris;

class NextPiecePreview : public Entity {
public:
    NextPiecePreview(Vector2f position)
        : nextType(TetrominoType::NONE),
          nextShape(),
          nextColor(),
          blockShape(),
          border(),
          label(),
          font() {

        this->position = position;
    }

    void onCreate() override {
        // Setup label
        this->font.loadFromFile("assets/fonts/sansation.ttf");
        label.setFont(font);
        label.setString("Next:");
        label.setCharacterSize(20);
        label.setFillColor(Color::White);
        label.setPosition(position);

        // Setup border for preview area
        border.setSize(Vector2f(120, 120));
        border.setPosition(position.x, position.y + 30);
        border.setFillColor(Color::Transparent);
        border.setOutlineThickness(2.0f);
        border.setOutlineColor(Color::White);

        // Setup block shape
        blockShape.setSize(Vector2f(BLOCK_SIZE - 1.0f, BLOCK_SIZE - 1.0f));
        blockShape.setOutlineThickness(1.0f);
        blockShape.setOutlineColor(Color(50, 50, 50));
    }

    void onDraw(RenderWindow& window) override {
        window.draw(label);
        window.draw(border);

        if (nextType == TetrominoType::NONE)
            return;

        // Calculate center offset for preview
        auto previewPos = Vector2f(this->position.x + 30, this->position.y + 60);

        // Draw the next piece
        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (this->nextShape[y][x] == 0)
                    continue;
                    
                auto posX = previewPos.x + x * BLOCK_SIZE;
                auto posY = previewPos.y + y * BLOCK_SIZE;
                this->blockShape.setPosition(posX, posY);
                this->blockShape.setFillColor(this->nextColor);
                window.draw(this->blockShape);
            }
        }
    }

    void setNextPiece(TetrominoType type) {
        this->nextType = type;
        this->nextShape = getBaseShape(type);
        this->nextColor = getTetrominoColor(type);
    }

    auto getNextType() const { return this->nextType; }

private:
    TetrominoType nextType;
    ShapeMatrix nextShape;
    Color nextColor;
    RectangleShape blockShape;
    RectangleShape border;
    Text label;
    Font font;
};
