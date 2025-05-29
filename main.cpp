#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <iostream>
using namespace sf;

int main() {
    const int windowWidth = 800;
    const int windowHeight = 600;
    const int roadWidth = 500;
    const int laneCount = 4;
    const float laneWidth = static_cast<float>(roadWidth) / laneCount;

    RenderWindow window(VideoMode({windowWidth, windowHeight}), "Cars");
    window.setFramerateLimit(60);

    RectangleShape road(Vector2f(static_cast<float>(roadWidth), static_cast<float>(windowHeight)));
    road.setFillColor(Color(50, 50, 50));
    road.setPosition(Vector2f(0.f, 0.f));

    RectangleShape menu(Vector2f(static_cast<float>(windowWidth - roadWidth), static_cast<float>(windowHeight)));
    menu.setFillColor(Color(30, 30, 30));
    menu.setPosition(Vector2f(static_cast<float>(roadWidth), 0.f));

    // Création des lignes en tirets
    std::vector<std::vector<RectangleShape>> dashedLaneLines;
    const float dashLength = 40.f;  // Longueur d'un tiret
    const float gapLength = 20.f;   // Longueur de l'espace entre les tirets
    const float lineWidth = 4.f;   // Largeur de la ligne
    
    for (int i = 1; i < laneCount; ++i) {
        std::vector<RectangleShape> dashesForThisLane;
        float currentY = 0.f;
        
        while (currentY < windowHeight) {
            // Créer un tiret
            RectangleShape dash(Vector2f(lineWidth, dashLength));
            dash.setFillColor(Color::White);
            dash.setPosition(Vector2f(i * laneWidth - lineWidth / 2.f, currentY));
            dashesForThisLane.push_back(dash);
            
            // Passer au prochain tiret (tiret + espace)
            currentY += dashLength + gapLength;
        }
        
        dashedLaneLines.push_back(dashesForThisLane);
    }

    Texture carTexture;
    if(!carTexture.loadFromFile("assets/voiture.png")) {
        std::cout << "Erreur : impossible de charger l'image" << std::endl;
        return -1;
    }

    Sprite playerCar(carTexture);
    playerCar.setScale(Vector2f(0.5f, 0.5f));

    int currentLane = 1;

    // Correction : utiliser size.x et size.y au lieu de position.x et position.y
    float carWidth = playerCar.getGlobalBounds().size.x;
    float carHeight = playerCar.getGlobalBounds().size.y;
    
    float carX = currentLane * laneWidth + (laneWidth - carWidth) / 2.f;
    float carY = windowHeight - carHeight - 30.f;

    playerCar.setPosition(Vector2f(carX, carY));

    while (window.isOpen()) {
        while (std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        float dashSpeed = 3.0f;
        for (auto& laneOfDashes : dashedLaneLines) {
            for (auto& dash : laneOfDashes) {
                dash.move(Vector2f(0.f, dashSpeed));
                if (dash.getPosition().y > windowHeight) {
                    dash.setPosition(Vector2f(dash.getPosition().x, -dash.getSize().y));
                }
            }
        }


        window.clear();
        window.draw(road);

        for (const auto& laneOfDashes : dashedLaneLines) {
            for (const auto& dash : laneOfDashes) {
                window.draw(dash);
            }
        }

        window.draw(playerCar);
        window.draw(menu);
        window.display();
    }

    return 0;
}