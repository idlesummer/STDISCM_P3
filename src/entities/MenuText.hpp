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
        text.setString(content);
        text.setCharacterSize(size);
        text.setFillColor(Color::White);
        text.setPosition(position);
    }

    void onCreate() override {
        // Try to load font (will use default if fails)
        if (!font.loadFromFile("assets/sansation.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
        text.setFont(font);
    }

    void onDraw(RenderWindow& window) override {
        window.draw(text);
    }

private:
    Font font;
    Text text;
};
