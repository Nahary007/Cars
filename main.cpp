#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "PlayerCar.hpp"
#include "EnemyCar.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <vector>

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
    const float dashLength = 40.f;
    const float gapLength = 20.f;
    const float lineWidth = 4.f;
    
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
    if(!carTexture.loadFromFile("assets/joueur/voiture.png")) {
        std::cout << "Erreur : impossible de charger l'image" << std::endl;
        return -1;
    }

    PlayerCar player(carTexture, laneCount, laneWidth, static_cast<float>(windowHeight));

    // Declare enemyTextures in the correct scope
    std::vector<Texture> enemyTextures(4);
    if (!enemyTextures[0].loadFromFile("assets/enemie/enemi1.png") ||
        !enemyTextures[1].loadFromFile("assets/enemie/enemi2.png") ||
        !enemyTextures[2].loadFromFile("assets/enemie/enemi3.png") ||
        !enemyTextures[3].loadFromFile("assets/enemie/enemi5.png")) {
        std::cerr << "Erreur chargement textures ennemies" << std::endl;
        return -1;
    }


    std::vector<EnemyCar> enemies;
    float spawnTimer = 0.f;
    float spawnDelay = 1.5f; // secondes
    std::srand(static_cast<unsigned>(std::time(nullptr)));


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

        spawnTimer += 1.f / 60.f;
        if(spawnTimer >= spawnDelay) {
            spawnTimer = 0.f;
            int lane = rand() % laneCount;
            int typeIndex = rand() % 4;
            float laneX = lane * laneWidth + (laneWidth - enemyTextures[typeIndex].getSize().x * 0.5f) / 2.f;
            enemies.emplace_back(enemyTextures[typeIndex], laneX, 3.0f);
        }

        for (auto& enemy : enemies) {
            enemy.update();
        }

        // Suppression ennemis hors écran
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [&](const EnemyCar& e) {
                return e.getBounds().size.y > windowHeight;
            }), enemies.end());

        player.handleInput();
        player.update();

        window.clear();
        window.draw(road);

        for (const auto& laneOfDashes : dashedLaneLines) {
            for (const auto& dash : laneOfDashes) {
                window.draw(dash);
            }
        }

        player.draw(window);

        for (auto& enemy : enemies)
            enemy.draw(window);

        window.draw(menu);
        window.display();
    }

    return 0;
}