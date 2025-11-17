#pragma once

#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;


// Simple text entity for menu displays
class MenuText : public Entity {
public:
    MenuText(const string& content, Vector2f position, int size = 48)
        : Entity("MenuText") {
        this->text.setString(content);
        this->text.setCharacterSize(size);
        this->text.setFillColor(Color::White);
        this->text.setPosition(position);
    }

    void onCreate() override {
        // Try to load font (will use default if fails)
        if (!this->font.loadFromFile("assets/sansation.ttf"))
            this->font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
        this->text.setFont(this->font);
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->text);
    }

private:
    Font font;
    Text text;
};
