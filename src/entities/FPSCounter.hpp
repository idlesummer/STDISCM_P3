#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <sstream>
#include <iomanip>

using namespace sf;
using namespace std;

// FPS counter entity that displays frames per second
class FPSCounter : public Entity {
private:
    Font font;
    Text text;
    Time frameTime;
    int frameCount;
    float fps;

public:
    FPSCounter(Vector2f position)
        : Entity("FPSCounter", position),
          font(),
          text(),
          frameTime(Time::Zero),
          frameCount(0),
          fps(0.0f) {

        this->text.setCharacterSize(16);
        this->text.setFillColor(Color(200, 200, 200));
        this->text.setPosition(position);
        this->text.setString("FPS: 0");
    }

    void onCreate() override {
        this->font.loadFromFile("assets/fonts/sansation.ttf");
        this->text.setFont(this->font);
    }

    void onUpdate(Time deltaTime) override {
        this->frameTime += deltaTime;
        this->frameCount++;

        // Update FPS every 0.5 seconds
        if (this->frameTime.asSeconds() >= 0.5f) {
            this->fps = this->frameCount / this->frameTime.asSeconds();

            ostringstream ss;
            ss << "FPS: " << fixed << setprecision(0) << this->fps;
            this->text.setString(ss.str());

            this->frameTime = Time::Zero;
            this->frameCount = 0;
        }
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->text);
    }
};
