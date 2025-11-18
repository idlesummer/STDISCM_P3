#pragma once

// Pure Tetris scoring and level logic - no external dependencies
// Handles score calculation, level progression, and difficulty

class TetrisScoring {
private:
    int score;
    int lines;
    int level;

public:
    TetrisScoring()
        : score(0),
          lines(0),
          level(1) {
    }

    // Reset scoring to initial state
    void reset() {
        this->score = 0;
        this->lines = 0;
        this->level = 1;
    }

    // Add score directly
    void addScore(int points) {
        this->score += points;
    }

    // Add cleared lines and calculate score
    // Returns the points awarded
    int addLines(int linesCleared) {
        if (linesCleared <= 0)
            return 0;

        this->lines += linesCleared;

        // Calculate points based on lines cleared
        int points = 0;
        switch (linesCleared) {
            case 1:
                points = 100 * this->level;
                break;
            case 2:
                points = 300 * this->level;
                break;
            case 3:
                points = 500 * this->level;
                break;
            case 4:
                points = 800 * this->level; // Tetris!
                break;
            default:
                // For more than 4 lines (shouldn't happen in standard Tetris)
                points = 800 * this->level * (linesCleared - 3);
                break;
        }

        this->score += points;

        // Update level (every 10 lines)
        this->level = 1 + (this->lines / 10);

        return points;
    }

    // Calculate fall speed based on level
    // Returns fall interval in seconds
    float getFallInterval() const {
        // Start at 1.0 second, decrease by 0.1s per level
        // Minimum 0.1 seconds at level 10+
        float interval = 1.0f - (this->level - 1) * 0.1f;
        return (interval < 0.1f) ? 0.1f : interval;
    }

    // Getters
    int getScore() const {
        return this->score;
    }

    int getLines() const {
        return this->lines;
    }

    int getLevel() const {
        return this->level;
    }

    // Set level manually (useful for starting at different difficulties)
    void setLevel(int newLevel) {
        if (newLevel > 0) {
            this->level = newLevel;
        }
    }
};
