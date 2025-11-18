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
        : nextType(TetrominoType::NONE) {
        this->position = position;
    }

    void onCreate() override {
        // Load font
        if (!this->font.loadFromFile("assets/fonts/sansation.ttf"))
            this->font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

        // Setup label
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

        if (nextType != TetrominoType::NONE) {
            // Calculate center offset for preview
            Vector2f previewPos(position.x + 30, position.y + 60);

            // Draw the next piece
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    if (nextShape[y][x] != 0) {
                        blockShape.setPosition(
                            previewPos.x + x * BLOCK_SIZE,
                            previewPos.y + y * BLOCK_SIZE
                        );
                        blockShape.setFillColor(nextColor);
                        window.draw(blockShape);
                    }
                }
            }
        }
    }

    void setNextPiece(TetrominoType type) {
        nextType = type;
        nextShape = getBaseShape(type);
        nextColor = getTetrominoColor(type);
    }

    TetrominoType getNextType() const {
        return nextType;
    }
};
