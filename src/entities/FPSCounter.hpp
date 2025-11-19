#pragma once
#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace sf;
using namespace std;
using namespace std::chrono;

// FPS counter entity that displays frames per second
class FPSCounter : public Entity {
private:
    Font font;
    Text text;
    steady_clock::time_point lastUpdate;
    int frameCount;
    float fps;

public:
    FPSCounter(Vector2f position)
        : Entity("FPSCounter", position),
          font(),
          text(),
          lastUpdate(steady_clock::now()),
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
        this->lastUpdate = steady_clock::now();
    }

    void onUpdate(Time deltaTime) override {
        this->frameCount++;

        // Calculate elapsed time using steady_clock
        auto now = steady_clock::now();
        auto elapsed = duration_cast<duration<float>>(now - this->lastUpdate).count();

        // Only update FPS every 0.5 seconds
        if (elapsed < 0.5f)
            return;

        auto ss = ostringstream();
        this->fps = this->frameCount / elapsed;
        ss << "FPS: " << fixed << setprecision(0) << this->fps;
        this->text.setString(ss.str());

        this->lastUpdate = now;
        this->frameCount = 0;
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->text);
    }
};
