#pragma once
#include "../core/Entity.hpp"
#include "../game/tetris/TetrisScoring.hpp"
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

class TetrisScoreText : public Entity {
private:
    TetrisScoring tetrisScoring; // Pure game logic
    Text linesText;
    Font font;

public:
    TetrisScoreText(Vector2f position)
        : Entity("TetrisScoreText", position),
          tetrisScoring(),
          linesText(),
          font() {
    }

    void onCreate() override {
        // Lines text
        this->font.loadFromFile("assets/fonts/sansation.ttf");
        this->linesText.setFont(this->font);
        this->linesText.setCharacterSize(20);
        this->linesText.setFillColor(Color::White);
        this->linesText.setPosition(this->position);

        this->updateDisplay();
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->linesText);
    }

    void addLines(int linesCleared) {
        this->tetrisScoring.addLines(linesCleared);
        this->updateDisplay();
    }

    void reset() {
        this->tetrisScoring.reset();
        this->updateDisplay();
    }

    int getLines() const { return this->tetrisScoring.getLines(); }

    // Access to underlying game logic (if needed)
    TetrisScoring& getTetrisScoring() { return this->tetrisScoring; }
    const TetrisScoring& getTetrisScoring() const { return this->tetrisScoring; }

private:
    void updateDisplay() {
        this->linesText.setString("Lines: " + to_string(this->tetrisScoring.getLines()));
    }
};
