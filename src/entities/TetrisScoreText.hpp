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
        : score(0), lines(0), level(1) {
        this->position = position;
    }

    void onCreate() override {
        // Load font
        if (!this->font.loadFromFile("assets/fonts/sansation.ttf"))
            this->font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

        // Score text
        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(position);

        // Lines text
        linesText.setFont(font);
        linesText.setCharacterSize(20);
        linesText.setFillColor(Color::White);
        linesText.setPosition(position.x, position.y + 30);

        // Level text
        levelText.setFont(font);
        levelText.setCharacterSize(20);
        levelText.setFillColor(Color::White);
        levelText.setPosition(position.x, position.y + 60);

        updateDisplay();
    }

    void onDraw(RenderWindow& window) override {
        window.draw(scoreText);
        window.draw(linesText);
        window.draw(levelText);
    }

    void addScore(int points) {
        score += points;
        updateDisplay();
    }

    void addLines(int linesCleared) {
        lines += linesCleared;

        // Award points based on lines cleared
        switch (linesCleared) {
            case 1: addScore(100 * level); break;
            case 2: addScore(300 * level); break;
            case 3: addScore(500 * level); break;
            case 4: addScore(800 * level); break; // Tetris!
        }

        // Level up every 10 lines
        level = 1 + (lines / 10);

        updateDisplay();
    }

    void reset() {
        score = 0;
        lines = 0;
        level = 1;
        updateDisplay();
    }

    int getScore() const { return score; }
    int getLines() const { return lines; }
    int getLevel() const { return level; }

private:
    void updateDisplay() {
        scoreText.setString("Score: " + to_string(score));
        linesText.setString("Lines: " + to_string(lines));
        levelText.setString("Level: " + to_string(level));
    }
};
