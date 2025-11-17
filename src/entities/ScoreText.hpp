#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

using namespace sf;
using namespace std;


// Score display that updates in real-time
class ScoreText : public Entity {
public:
    ScoreText(shared_ptr<Font> font) : Entity("ScoreText"), font(font), score(0) {}

    void onCreate() override {
        // Font is now loaded asynchronously by AssetManager
        if (this->font) {
            this->text.setFont(*this->font);
            this->text.setCharacterSize(24);
            this->text.setFillColor(Color::White);
            this->text.setPosition(10, 10);
            this->updateText();
        }
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

    shared_ptr<Font> font;
    Text text;
    float score;
};
