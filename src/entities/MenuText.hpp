#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;


// Simple text entity for menu displays
class MenuText : public Entity {
private:
    Font font;
    Text text;
    bool centered;

public:
    MenuText(const string& content, Vector2f position, int size = 48, bool centered = false)
        : Entity("MenuText", position),
          font(),
          text(),
          centered(centered) {

        this->text.setString(content);
        this->text.setCharacterSize(size);
        this->text.setFillColor(Color::White);
        this->text.setPosition(position);
    }

    void onCreate() override {
        this->font.loadFromFile("assets/fonts/sansation.ttf");
        this->text.setFont(this->font);

        // Center text if requested
        if (this->centered) {
            auto bounds = this->text.getLocalBounds();
            this->text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        }
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->text);
    }
};
