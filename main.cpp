#include <SFML/Graphics.hpp>
using namespace sf;
int main() {
    const int windowWidth = 800;
    const int windowHeight = 600;
    const int roadWidth = 500;
    const int laneCount = 4;
    const float laneWidth = static_cast<float>(roadWidth) / laneCount;

    // RenderWindow window(VideoMode(static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight)), "Car Game Retro");
    RenderWindow window(VideoMode({windowWidth, windowHeight}), "Cars");
    window.setFramerateLimit(60);

    RectangleShape road(Vector2f(static_cast<float>(roadWidth), static_cast<float>(windowHeight)));
    road.setFillColor(Color(50, 50, 50));
    road.setPosition(Vector2f(0.f, 0.f));

    RectangleShape menu(Vector2f(static_cast<float>(windowWidth - roadWidth), static_cast<float>(windowHeight)));
    menu.setFillColor(Color(30, 30, 30));
    menu.setPosition(Vector2f(static_cast<float>(roadWidth), 0.f));

    std::vector<RectangleShape> laneLines;
    for (int i = 1; i < laneCount; ++i) {
        RectangleShape line(Vector2f(4.f, static_cast<float>(windowHeight)));
        line.setFillColor(Color::White);
        line.setPosition(Vector2f(i * laneWidth - 2.f, 0.f));
        laneLines.push_back(line);
    }

    while (window.isOpen()) {

        while (std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        window.draw(road);

        for (auto& line : laneLines)
            window.draw(line);

        window.draw(menu);
        window.display();
    }

    return 0;
}
