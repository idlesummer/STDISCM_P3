#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;
#include <string>


// Score display that updates in real-time
class ScoreText : public Entity {
public:
    ScoreText() : Entity("ScoreText"), score(0) {}

    void onCreate() override {
        if (!this->font.loadFromFile("assets/sansation.ttf"))
            this->font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

        this->text.setFont(this->font);
        this->text.setCharacterSize(24);
        this->text.setFillColor(Color::White);
        this->text.setPosition(10, 10);
        this->updateText();
    }

    void onUpdate(Time dt) override {
        this->score += dt.asSeconds() * 10;  // Score increases over time
        this->updateText();
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->text);
    }

    int getScore() const { return static_cast<int>(this->score); }

private:
    void updateText() {
        this->text.setString("Score: " + to_string(static_cast<int>(this->score)));
    }

    Font font;
    Text text;
    float score;
};
