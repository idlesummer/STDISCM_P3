#include <iostream>
#include <SFML/Graphics.hpp>

// Ultra-minimal test - no React architecture, just SFML
int main() {
    std::cout << "=== Minimal SFML Test ===" << std::endl;
    std::cout << "Use WASD to move the circle" << std::endl;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Minimal SFML");
    window.setFramerateLimit(60);

    sf::CircleShape circle(30);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition(400, 300);
    circle.setOrigin(30, 30);  // Center the circle

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                float speed = 10.0f;
                sf::Vector2f pos = circle.getPosition();

                switch (event.key.code) {
                    case sf::Keyboard::A:
                    case sf::Keyboard::Left:
                        pos.x -= speed;
                        std::cout << "Left - new X: " << pos.x << std::endl;
                        break;
                    case sf::Keyboard::D:
                    case sf::Keyboard::Right:
                        pos.x += speed;
                        std::cout << "Right - new X: " << pos.x << std::endl;
                        break;
                    case sf::Keyboard::W:
                    case sf::Keyboard::Up:
                        pos.y -= speed;
                        std::cout << "Up - new Y: " << pos.y << std::endl;
                        break;
                    case sf::Keyboard::S:
                    case sf::Keyboard::Down:
                        pos.y += speed;
                        std::cout << "Down - new Y: " << pos.y << std::endl;
                        break;
                }

                circle.setPosition(pos);
            }
        }

        window.clear(sf::Color(20, 20, 40));
        window.draw(circle);
        window.display();
    }

    return 0;
}
