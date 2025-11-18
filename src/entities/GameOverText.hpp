#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;


// Game Over text display with customizable content, position, size, and color
class GameOverText : public Entity {
private:
    Font font;
    Text text;
    string content;
    int size;
    Color color;

public:
    GameOverText(const string& content, Vector2f position, int size, Color color)
        : Entity("GameOverText", position),
            font(),
            text(),
            content(content),
            size(size),
            color(color) {}

    void onCreate() override {
        this->font.loadFromFile("assets/fonts/sansation.ttf");
        this->text.setFont(this->font);
        this->text.setString(this->content);
        this->text.setCharacterSize(this->size);
        this->text.setFillColor(this->color);
        this->text.setPosition(this->position);

        // Center the text
        auto bounds = this->text.getLocalBounds();
        this->text.setOrigin(bounds.width / 2, bounds.height / 2);
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->text);
    }
};
