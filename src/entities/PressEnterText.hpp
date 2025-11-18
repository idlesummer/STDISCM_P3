#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;


// Pulsing "Press Enter" instruction text with animation
class PressEnterText : public Entity {
private:
    // --- Fields ---
    Font font;
    Text text;
    Vector2f position;
    float alpha;
    int fadeDirection;

public:
    // --- Constructor ---
    PressEnterText(Vector2f position)
        : Entity("PressEnterText"), position(position), alpha(255), fadeDirection(-1) {}

    // --- Methods ---
    void onCreate() override {
        this->font.loadFromFile("assets/fonts/sansation.ttf");
        this->text.setFont(this->font);
        this->text.setString("Press ENTER to Start");
        this->text.setCharacterSize(24);
        this->text.setPosition(this->position);

        // Center the text
        auto bounds = this->text.getLocalBounds();
        this->text.setOrigin(bounds.width / 2, bounds.height / 2);
    }

    void onUpdate(Time dt) override {
        // Pulse effect
        this->alpha += this->fadeDirection * 200 * dt.asSeconds();

        if (this->alpha <= 100) {
            this->alpha = 100;
            this->fadeDirection = 1;
        } else if (this->alpha >= 255) {
            this->alpha = 255;
            this->fadeDirection = -1;
        }

        this->text.setFillColor(Color(255, 255, 255, static_cast<Uint8>(this->alpha)));
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->text);
    }
};
