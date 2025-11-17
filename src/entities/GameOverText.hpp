#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

using namespace sf;
using namespace std;


// Game Over text display with customizable content, position, size, and color
class GameOverText : public Entity {
public:
    GameOverText(const string& content, Vector2f position, shared_ptr<Font> font, int size, Color color)
        : Entity("GameOverText"), font(font) {
        this->content = content;
        this->position = position;
        this->size = size;
        this->color = color;
    }

    void onCreate() override {
        // Font is now loaded asynchronously by AssetManager
        if (this->font) {
            this->text.setFont(*this->font);
            this->text.setString(this->content);
            this->text.setCharacterSize(this->size);
            this->text.setFillColor(this->color);
            this->text.setPosition(this->position);

            // Center the text
            auto bounds = this->text.getLocalBounds();
            this->text.setOrigin(bounds.width / 2, bounds.height / 2);
        }
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->text);
    }

private:
    shared_ptr<Font> font;
    Text text;
    string content;
    Vector2f position;
    int size;
    Color color;
};
