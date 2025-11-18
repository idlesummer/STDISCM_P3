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

public:
  MenuText(const string& content, Vector2f position, int size = 48)
    : Entity("MenuText", position),
      font(),
      text() {

    this->text.setString(content);
    this->text.setCharacterSize(size);
    this->text.setFillColor(Color::White);
    this->text.setPosition(position);
  }

  void onCreate() override {
    this->font.loadFromFile("assets/fonts/sansation.ttf");
    this->text.setFont(this->font);
  }

  void onDraw(RenderWindow& window) override {
    window.draw(this->text);
  }
};
