#pragma once

#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;


// Pulsing "Press Enter" instruction text with animation
class PressEnterText : public Entity {
public:
    PressEnterText(Vector2f position)
        : Entity("PressEnterText"), position(position), alpha(255), fadeDirection(-1) {}

    void onCreate() override {
        if (!font.loadFromFile("assets/sansation.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

        text.setFont(font);
        text.setString("Press ENTER to Start");
        text.setCharacterSize(24);
        text.setPosition(position);

        // Center the text
        auto bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2, bounds.height / 2);
    }

    void onUpdate(Time dt) override {
        // Pulse effect
        alpha += fadeDirection * 200 * dt.asSeconds();

        if (alpha <= 100) {
            alpha = 100;
            fadeDirection = 1;
        } else if (alpha >= 255) {
            alpha = 255;
            fadeDirection = -1;
        }

        text.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
    }

    void onDraw(RenderWindow& window) override {
        window.draw(text);
    }

private:
    Font font;
    Text text;
    Vector2f position;
    float alpha;
    int fadeDirection;
};
