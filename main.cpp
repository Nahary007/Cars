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

bool showMenu(RenderWindow& window) {
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/bg.png")) {
        return false;
    }
    Sprite backgroundSprite(backgroundTexture);

    Font font;
    if (!font.openFromFile("assets/fonts/arial.ttf")) {
        return false;
    }

    Text title(font, "Cars", 48);
    title.setFillColor(Color::Yellow);
    title.setPosition(Vector2f(200.f, 100.f));

    Text startButton(font, "Start", 36);
    startButton.setFillColor(Color::White);
    startButton.setPosition(Vector2f(350.f, 250.f));

    Text quitButton(font, "Quit", 36);
    quitButton.setFillColor(Color::White);
    quitButton.setPosition(Vector2f(350.f, 320.f));

    while (window.isOpen()) {
        while (std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }

            // Correction : utilisation de la nouvelle syntaxe SFML 3.0.0
            if (event->is<Event::MouseButtonPressed>()) {
                const auto& mouseButton = event->getIf<Event::MouseButtonPressed>();
                if (mouseButton->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(Vector2i(mouseButton->position.x, mouseButton->position.y));

                    if (startButton.getGlobalBounds().contains(mousePos)) {
                        return true; // L'utilisateur a cliqué sur Start
                    } else if (quitButton.getGlobalBounds().contains(mousePos)) {
                        window.close();
                        return false;
                    }
                }
            }
        }
        
        window.clear();
        window.draw(backgroundSprite);
        window.draw(title);
        window.draw(startButton);
        window.draw(quitButton);
        window.display();
    }

    return false;
}

bool showGameOver(RenderWindow& window) {
    Font font;
    if (!font.openFromFile("assets/fonts/arial.ttf")) {
        return false;
    }

    Text gameOverText(font,"Game Over", 48);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setPosition(Vector2f(250.f, 150.f));

    Text retryButton(font,"Rejouer", 36);
    retryButton.setFillColor(Color::White);
    retryButton.setPosition(Vector2f(320.f, 250.f));

    Text quitButton(font,"Quitter", 36);
    quitButton.setFillColor(Color::White);
    quitButton.setPosition(Vector2f(320.f, 320.f));

    while (window.isOpen()) {
        while (std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
                return false;
            }

            if (event->is<Event::MouseButtonPressed>()) {
                const auto& mouseButton = event->getIf<Event::MouseButtonPressed>();
                if (mouseButton->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(Vector2i(mouseButton->position.x, mouseButton->position.y));

                    if (retryButton.getGlobalBounds().contains(mousePos)) {
                        return true;
                    } else if (quitButton.getGlobalBounds().contains(mousePos)) {
                        window.close();
                        return false;
                    }
                }
            }
        }

        window.clear(Color::Black);
        window.draw(gameOverText);
        window.draw(retryButton);
        window.draw(quitButton);
        window.display();
    }

    return false;
}

void showExplosion(RenderWindow& window, Vector2f position) {
    Texture explosionTexture;
    if (!explosionTexture.loadFromFile("assets/boom.png")) {
        std::cerr << "Erreur chargement explosion.png" << std::endl;
        return;
    }

    Sprite explosionSprite(explosionTexture);
    explosionSprite.setOrigin(Vector2f(explosionTexture.getSize().x / 2.f, explosionTexture.getSize().y / 2.f));
    explosionSprite.setPosition(position);

    window.clear();
    window.draw(explosionSprite);
    window.display();

    Clock waitClock;
    while (waitClock.getElapsedTime().asSeconds() < 2.0f) {
        while (std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }
    }
}



int main() {
    const int windowWidth = 800;
    const int windowHeight = 600;
    const int roadWidth = 500;
    const int laneCount = 4;
    const float laneWidth = static_cast<float>(roadWidth) / laneCount;
    int level = 1;
    float levelTimer = 0.f;
    float levelDuration = 10.f;
    float enemySpeed = 3.f;

    RenderWindow window(VideoMode({windowWidth, windowHeight}), "Cars");
    window.setFramerateLimit(60);

  
    if (!showMenu(window)) {
        return 0;
    }

    sf::Clock clock;

    RectangleShape road(Vector2f(static_cast<float>(roadWidth), static_cast<float>(windowHeight)));
    road.setFillColor(Color(50, 50, 50));
    road.setPosition(Vector2f(0.f, 0.f));

    RectangleShape menu(Vector2f(static_cast<float>(windowWidth - roadWidth), static_cast<float>(windowHeight)));
    menu.setFillColor(Color(30, 30, 30));
    menu.setPosition(Vector2f(static_cast<float>(roadWidth), 0.f));

    std::vector<std::vector<RectangleShape>> dashedLaneLines;
    const float dashLength = 40.f;
    const float gapLength = 20.f;
    const float lineWidth = 4.f;

    for (int i = 1; i < laneCount; ++i) {
        std::vector<RectangleShape> dashesForThisLane;
        float currentY = 0.f;

        while (currentY < windowHeight) {
            RectangleShape dash(Vector2f(lineWidth, dashLength));
            dash.setFillColor(Color::White);
            dash.setPosition(Vector2f(i * laneWidth - lineWidth / 2.f, currentY));
            dashesForThisLane.push_back(dash);
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

    std::vector<Texture> enemyTextures(4);
    if (!enemyTextures[0].loadFromFile("assets/enemie/enemi4.png") ||
        !enemyTextures[1].loadFromFile("assets/enemie/enemi2.png") ||
        !enemyTextures[2].loadFromFile("assets/enemie/enemi3.png") ||
        !enemyTextures[3].loadFromFile("assets/enemie/enemi5.png")) {
        std::cerr << "Erreur chargement textures ennemies" << std::endl;
        return -1;
    }

    std::vector<EnemyCar> enemies;
    float spawnTimer = 0.f;
    float spawnDelay = 1.5f;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    Font font;
    if (!font.openFromFile("assets/fonts/Gatrich.otf")) {
        std::cout << "Font introuvable" << std::endl;
    }

    Text levelText(font, "Niveau: 1", 24);
    Text jumpText(font, "Nombre saut: 5 ", 24);

    levelText.setPosition(Vector2f(static_cast<float>(roadWidth) + 20.f, 20.f));
    levelText.setFillColor(Color::White);

    jumpText.setPosition(Vector2f(static_cast<float>(roadWidth) + 20.f, 100.f));
    jumpText.setFillColor(Color::White);

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        levelTimer += deltaTime;
        if (levelTimer >= levelDuration) {
            level++;
            levelTimer = 0.f;
            enemySpeed += 0.5f;
            player.resetJumpCount();

            levelText.setString("Niveau: " + std::to_string(level));
            
            std::cout << "🚀 Niveau atteint : " << level << ", Vitesse ennemie: " << enemySpeed << std::endl;
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

        spawnTimer += deltaTime;
        if (spawnTimer >= spawnDelay) {
            spawnTimer = 0.f;

            int lane = rand() % laneCount;
            int typeIndex = rand() % 4;

            float carWidth = enemyTextures[typeIndex].getSize().x * 0.5f;
            float laneX = lane * laneWidth + (laneWidth - carWidth) / 2.f;

            enemies.emplace_back(enemyTextures[typeIndex], laneX, enemySpeed);
        }

        for (auto& enemy : enemies) {
            enemy.update();
        }

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [&](const EnemyCar& e) {
                return e.getBounds().position.y > windowHeight;
            }), enemies.end());

        bool collisionDetected = false;
        for (const auto& enemy : enemies) {
            if (!player.isJumping()) {
                sf::FloatRect playerBounds = player.getBounds();
                sf::FloatRect enemyBounds = enemy.getBounds();

                if (playerBounds.position.x < enemyBounds.position.x + enemyBounds.size.x &&
                    playerBounds.position.x + playerBounds.size.x > enemyBounds.position.x &&
                    playerBounds.position.y < enemyBounds.position.y + enemyBounds.size.y &&
                    playerBounds.position.y + playerBounds.size.y > enemyBounds.position.y) {
                    collisionDetected = true;
                    break;
                }
            }
        }

        if (collisionDetected) {
            std::cout << "💥 Collision détectée ! Explosion !" << std::endl;

            showExplosion(window, player.getBounds().position);

            bool retry = showGameOver(window);
            if (retry) {
                main();
            } else {
                window.close();
            }
            return 0;
        }



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

        jumpText.setString("Sauts restants: " + std::to_string(player.getJumpCount()));

        window.draw(menu);
        window.draw(jumpText);
        window.draw(levelText);
        window.display();
    }

    return 0;
}