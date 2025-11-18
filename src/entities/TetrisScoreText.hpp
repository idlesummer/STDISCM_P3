#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

class TetrisScoreText : public Entity {
public:
    TetrisScoreText(Vector2f position)
        : score(0), 
          lines(0), 
          level(1) {

        this->position = position;
    }

    void onCreate() override {
        // Score text
        this->font.loadFromFile("assets/fonts/sansation.ttf");
        scoreText.setFont(this->font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(position);

        // Lines text
        linesText.setFont(this->font);
        linesText.setCharacterSize(20);
        linesText.setFillColor(Color::White);
        linesText.setPosition(position.x, position.y + 30);

        // Level text
        levelText.setFont(this->font);
        levelText.setCharacterSize(20);
        levelText.setFillColor(Color::White);
        levelText.setPosition(position.x, position.y + 60);

        updateDisplay();
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->scoreText);
        window.draw(this->linesText);
        window.draw(this->levelText);
    }

    void addScore(int points) {
        this->score += points;
        updateDisplay();
    }

    void addLines(int linesCleared) {
        this->lines += linesCleared;

        // Award points based on lines cleared
        switch (linesCleared) {
            case 1: addScore(100 * this->level); break;
            case 2: addScore(300 * this->level); break;
            case 3: addScore(500 * this->level); break;
            case 4: addScore(800 * this->level); break; // Tetris!
        }

        // Level up every 10 lines
        this->level = 1 + (this->lines / 10);
        updateDisplay();
    }

    void reset() {
        this->score = 0;
        this->lines = 0;
        this->level = 1;
        updateDisplay();
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

    Text scoreText;
    Text linesText;
    Text levelText;
    int score;
    int lines;
    int level;
    Font font;
};
