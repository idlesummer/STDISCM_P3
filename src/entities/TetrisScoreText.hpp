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
    Text scoreText;
    Text linesText;
    Text levelText;
    Font font;

public:
    TetrisScoreText(Vector2f position)
        : Entity("TetrisScoreText", position),
          tetrisScoring(),
          scoreText(),
          linesText(),
          levelText(),
          font() {
    }

    void onCreate() override {
        // Score text
        this->font.loadFromFile("assets/fonts/sansation.ttf");
        this->scoreText.setFont(this->font);
        this->scoreText.setCharacterSize(20);
        this->scoreText.setFillColor(Color::White);
        this->scoreText.setPosition(this->position);

        // Lines text
        this->linesText.setFont(this->font);
        this->linesText.setCharacterSize(20);
        this->linesText.setFillColor(Color::White);
        this->linesText.setPosition(this->position.x, this->position.y + 30);

        // Level text
        this->levelText.setFont(this->font);
        this->levelText.setCharacterSize(20);
        this->levelText.setFillColor(Color::White);
        this->levelText.setPosition(this->position.x, this->position.y + 60);

        this->updateDisplay();
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->scoreText);
        window.draw(this->linesText);
        window.draw(this->levelText);
    }

    void addScore(int points) {
        this->tetrisScoring.addScore(points);
        this->updateDisplay();
    }

    void addLines(int linesCleared) {
        this->tetrisScoring.addLines(linesCleared);
        this->updateDisplay();
    }

    void reset() {
        this->tetrisScoring.reset();
        this->updateDisplay();
    }

    int getScore() const { return this->tetrisScoring.getScore(); }
    int getLines() const { return this->tetrisScoring.getLines(); }
    int getLevel() const { return this->tetrisScoring.getLevel(); }

    // Access to underlying game logic (if needed)
    TetrisScoring& getTetrisScoring() { return this->tetrisScoring; }
    const TetrisScoring& getTetrisScoring() const { return this->tetrisScoring; }

private:
    void updateDisplay() {
        this->scoreText.setString("Score: " + to_string(this->tetrisScoring.getScore()));
        this->linesText.setString("Lines: " + to_string(this->tetrisScoring.getLines()));
        this->levelText.setString("Level: " + to_string(this->tetrisScoring.getLevel()));
    }
};
