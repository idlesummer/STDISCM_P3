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
        this->label.setFont(this->font);
        this->label.setString("Next:");
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

        if (this->nextType == TetrominoType::NONE)
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
