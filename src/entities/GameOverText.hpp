#pragma once

#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;
#include <string>


// Game Over text display with customizable content, position, size, and color
class GameOverText : public Entity {
public:
    GameOverText(const string& content, Vector2f position, int size, Color color)
        : Entity("GameOverText") {
        this->content = content;
        this->position = position;
        this->size = size;
        this->color = color;
    }

    void onCreate() override {
        if (!font.loadFromFile("assets/sansation.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

        text.setFont(font);
        text.setString(content);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setPosition(position);

        // Center the text
        auto bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2, bounds.height / 2);
    }

    void onDraw(RenderWindow& window) override {
        window.draw(text);
    }

private:
    Font font;
    Text text;
    string content;
    Vector2f position;
    int size;
    Color color;
};
