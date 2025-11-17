#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

using namespace sf;
using namespace std;


// Simple text entity for menu displays
class MenuText : public Entity {
public:
    MenuText(const string& content, Vector2f position, shared_ptr<Font> font, int size = 48)
        : Entity("MenuText"), font(font) {
        this->text.setString(content);
        this->text.setCharacterSize(size);
        this->text.setFillColor(Color::White);
        this->text.setPosition(position);
    }

    void onCreate() override {
        // Font is now loaded asynchronously by AssetManager
        if (this->font) {
            this->text.setFont(*this->font);
        }
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->text);
    }

private:
    shared_ptr<Font> font;
    Text text;
};
