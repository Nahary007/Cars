#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

int main() {
    const int windowWidth = 800;
    const int windowHeight = 600;
    const int roadWidth = 500;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Car Game Retro");
    window.setFramerateLimit(60);

    sf::RectangleShape road(sf::Vector2f(static_cast<float>(roadWidth), static_cast<float>(windowHeight)));
    road.setFillColor(sf::Color(50, 50, 50));
    road.setPosition(0.f, 0.f);

    sf::RectangleShape menu(sf::Vector2f(static_cast<float>(windowWidth - roadWidth), static_cast<float>(windowHeight)));
    menu.setFillColor(sf::Color(30, 30, 30));
    menu.setPosition(static_cast<float>(roadWidth), 0.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(road);
        window.draw(menu);
        window.display();
    }

    return 0;
}
