#pragma once

// Pure Tetris lines tracking - no external dependencies
// Handles line clearing count

class TetrisScoring {
private:
    int lines;

public:
    TetrisScoring()
        : lines(0) {
    }

    // Reset lines to initial state
    void reset() {
        this->lines = 0;
    }

    // Add cleared lines
    void addLines(int linesCleared) {
        if (linesCleared > 0)
            this->lines += linesCleared;
    }

    // Getter
    auto getLines() const {
        return this->lines;
    }
};
