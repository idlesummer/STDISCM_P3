#pragma once
#include "../core/Entity.hpp"
#include "../utils/TetrominoShapes.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;
using namespace Tetris;

class NextPiecePreview : public Entity {
private:
    TetrominoType nextType;
    ShapeMatrix nextShape;
    Color nextColor;
    RectangleShape blockShape;
    RectangleShape border;
    Text label;
    Font font;

public:
    NextPiecePreview(Vector2f position)
        : Entity("NextPiecePreview", position),
          nextType(TetrominoType::NONE),
          nextShape(),
          nextColor(),
          blockShape(),
          border(),
          label(),
          font() {
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

        // Calculate bounding box of the piece
        int minX = 4, maxX = -1;
        int minY = 4, maxY = -1;

        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (this->nextShape[y][x] != 0) {
                    minX = min(minX, x);
                    maxX = max(maxX, x);
                    minY = min(minY, y);
                    maxY = max(maxY, y);
                }
            }
        }

        // Calculate piece dimensions
        int pieceWidth = (maxX - minX + 1) * BLOCK_SIZE;
        int pieceHeight = (maxY - minY + 1) * BLOCK_SIZE;

        // Calculate center offset for preview (120x120 border)
        float borderX = this->position.x;
        float borderY = this->position.y + 30;
        float borderWidth = 120;
        float borderHeight = 120;

        float offsetX = borderX + (borderWidth - pieceWidth) / 2.0f - minX * BLOCK_SIZE;
        float offsetY = borderY + (borderHeight - pieceHeight) / 2.0f - minY * BLOCK_SIZE;

        // Draw the next piece centered
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (this->nextShape[y][x] == 0)
                    continue;

                float posX = offsetX + x * BLOCK_SIZE;
                float posY = offsetY + y * BLOCK_SIZE;
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
};
