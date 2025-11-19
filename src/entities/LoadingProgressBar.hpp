#pragma once
#include "../core/Entity.hpp"
#include "../core/AssetManager.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace sf;

class LoadingProgressBar : public Entity {
private:
    Font font;
    Text percentageText;
    Text titleText;
    Text instructionText;
    RectangleShape barBackground;
    RectangleShape barForeground;

    float barWidth;
    float barHeight;

public:
    LoadingProgressBar(Vector2f position, float width = 200.f, float height = 30.f)
        : Entity("LoadingProgressBar", position),
          font(),
          percentageText(),
          titleText(),
          instructionText(),
          barBackground(),
          barForeground(),
          barWidth(width),
          barHeight(height) {
    }

    void onCreate() override {
        // Setup font
        this->font.loadFromFile("assets/fonts/sansation.ttf");

        // Setup title text
        this->titleText.setFont(this->font);
        this->titleText.setString("Loading Assets");
        this->titleText.setCharacterSize(16);
        this->titleText.setFillColor(Color::White);
        this->titleText.setPosition(this->position.x, this->position.y - 25);

        // Setup percentage text
        this->percentageText.setFont(this->font);
        this->percentageText.setCharacterSize(16);
        this->percentageText.setFillColor(Color::White);

        // Setup instruction text
        this->instructionText.setFont(this->font);
        this->instructionText.setString("Press Enter to Show Icons");
        this->instructionText.setCharacterSize(14);
        this->instructionText.setFillColor(Color(200, 200, 200));
        this->instructionText.setPosition(this->position.x, this->position.y + this->barHeight + 10);

        // Setup bar background (gray border/background)
        this->barBackground.setSize(Vector2f(this->barWidth, this->barHeight));
        this->barBackground.setPosition(this->position);
        this->barBackground.setFillColor(Color(50, 50, 50));
        this->barBackground.setOutlineColor(Color::White);
        this->barBackground.setOutlineThickness(2.f);

        // Setup bar foreground (green fill that grows)
        this->barForeground.setSize(Vector2f(0.f, this->barHeight));
        this->barForeground.setPosition(this->position);
        this->barForeground.setFillColor(Color(0, 200, 0));

        this->updateProgress();
    }

    void onUpdate(Time deltaTime) override {
        this->updateProgress();
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->titleText);
        window.draw(this->barBackground);
        window.draw(this->barForeground);
        window.draw(this->percentageText);

        // Show instruction text only when loading is complete
        auto& assetManager = AssetManager::getInstance();
        if (assetManager.isLoadingComplete()) {
            window.draw(this->instructionText);
        }
    }

private:
    void updateProgress() {
        auto& assetManager = AssetManager::getInstance();
        auto progress = assetManager.getLoadingProgress();
        auto loaded = assetManager.getLoadedTextureCount();
        auto total = assetManager.getTotalTextureCount();

        // Update bar fill
        this->barForeground.setSize(Vector2f(this->barWidth * progress, this->barHeight));

        // Update percentage text
        auto ss = stringstream();
        ss << fixed << setprecision(0) << (progress * 100.f) << "%";
        ss << " (" << loaded << "/" << total << ")";
        this->percentageText.setString(ss.str());

        // Position percentage text to the right of the bar
        auto textBounds = this->percentageText.getLocalBounds();
        this->percentageText.setPosition(
            this->position.x + this->barWidth + 15.f,
            this->position.y + (this->barHeight - textBounds.height) / 2.f - 5.f
        );
    }
};
