#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

class TetrisScoreText : public Entity {
private:
    Text scoreText;
    Text linesText;
    Text levelText;
    int score;
    int lines;
    int level;
    Font font;

public:
    TetrisScoreText(Vector2f position)
        : Entity("TetrisScoreText", position),
          scoreText(),
          linesText(),
          levelText(),
          score(0),
          lines(0),
          level(1),
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
        this->score += points;
        this->updateDisplay();
    }

    void addLines(int linesCleared) {
        this->lines += linesCleared;

        // Award points based on lines cleared
        switch (linesCleared) {
            case 1: this->addScore(100 * this->level); break;
            case 2: this->addScore(300 * this->level); break;
            case 3: this->addScore(500 * this->level); break;
            case 4: this->addScore(800 * this->level); break; // Tetris!
        }

        // Level up every 10 lines
        this->level = 1 + (this->lines / 10);
        this->updateDisplay();
    }

    void reset() {
        this->score = 0;
        this->lines = 0;
        this->level = 1;
        this->updateDisplay();
    }

    int getScore() const { return this->score; }
    int getLines() const { return this->lines; }
    int getLevel() const { return this->level; }

private:
    void updateDisplay() {
        this->scoreText.setString("Score: " + to_string(this->score));
        this->linesText.setString("Lines: " + to_string(this->lines));
        this->levelText.setString("Level: " + to_string(this->level));
    }
};
