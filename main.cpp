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
#include <fstream>
#include <cmath>

using namespace sf;

// Couleurs modernes
const Color DARK_BLUE = Color(15, 25, 45);
const Color NEON_CYAN = Color(0, 255, 255);
const Color NEON_GREEN = Color(57, 255, 20);
const Color NEON_PINK = Color(255, 20, 147);
const Color GOLD = Color(255, 215, 0);
const Color SILVER = Color(192, 192, 192);

// Fonction pour créer un effet de glow
RectangleShape createGlowEffect(Vector2f size, Vector2f position, Color color, float glowIntensity = 0.3f) {
    RectangleShape glow(size);
    glow.setPosition(position);
    Color glowColor = color;
    glowColor.a = static_cast<unsigned char>(255 * glowIntensity);
    glow.setFillColor(glowColor);
    return glow;
}

// Classe pour les particules d'étoiles
class StarField {
private:
    std::vector<CircleShape> stars;
    std::vector<float> speeds;
    int windowHeight;
    
public:
    StarField(int count, int height) : windowHeight(height) {
        stars.resize(count);
        speeds.resize(count);
        
        for (int i = 0; i < count; ++i) {
            stars[i].setRadius(rand() % 3 + 1);
            stars[i].setPosition(Vector2f(rand() % 500, rand() % height));
            stars[i].setFillColor(Color(255, 255, 255, 100 + rand() % 155));
            speeds[i] = (rand() % 50 + 10) / 10.0f;
        }
    }
    
    void update(float deltaTime) {
        for (size_t i = 0; i < stars.size(); ++i) {
            stars[i].move(Vector2f(0, speeds[i] * 100 * deltaTime));
            if (stars[i].getPosition().y > windowHeight) {
                stars[i].setPosition(Vector2f(rand() % 500, -10));
            }
        }
    }
    
    void draw(RenderWindow& window) {
        for (const auto& star : stars) {
            window.draw(star);
        }
    }
};

// Fonction pour créer un bouton stylisé
class StyledButton {
private:
    RectangleShape background;
    RectangleShape border;
    Text text;
    Color normalColor;
    Color hoverColor;
    bool isHovered;
    
public:
    StyledButton(const Font& font, const String& buttonText, Vector2f position, Vector2f size) 
        : text(font, buttonText, 24) {
        normalColor = Color(20, 30, 60, 200);
        hoverColor = NEON_CYAN;
        isHovered = false;
        
        background.setSize(size);
        background.setPosition(position);
        background.setFillColor(normalColor);
        
        border.setSize(size);
        border.setPosition(position);
        border.setFillColor(Color::Transparent);
        border.setOutlineThickness(2.0f);
        border.setOutlineColor(NEON_CYAN);
        
        text.setFillColor(Color::White);
        
        // Centrer le texte
        FloatRect textBounds = text.getLocalBounds();
        text.setPosition(
            Vector2f(
            position.x + (size.x - textBounds.size.x) / 2 - textBounds.position.x,
            position.y + (size.y - textBounds.size.y) / 2 - textBounds.position.y - 5
            )
        );
    }
    
    void update(Vector2f mousePos) {
        isHovered = background.getGlobalBounds().contains(mousePos);
        
        if (isHovered) {
            background.setFillColor(Color(hoverColor.r, hoverColor.g, hoverColor.b, 50));
            border.setOutlineColor(hoverColor);
            text.setFillColor(hoverColor);
        } else {
            background.setFillColor(normalColor);
            border.setOutlineColor(NEON_CYAN);
            text.setFillColor(Color::White);
        }
    }
    
    bool isClicked(Vector2f mousePos) {
        return background.getGlobalBounds().contains(mousePos);
    }
    
    void draw(RenderWindow& window) {
        window.draw(background);
        window.draw(border);
        window.draw(text);
    }
};

int loadHighScore() {
    std::ifstream file("highscore.txt");
    int highscore = 0;
    if (file.is_open()) {
        file >> highscore;
        file.close();
    }
    return highscore;
}

void saveHighScore(int score) {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << score;
        file.close();
    }
}

bool showMenu(RenderWindow& window) {
    Font font;
    if (!font.openFromFile("assets/fonts/arial.ttf")) {
        return false;
    }

    StarField starField(50, 600);
    int highScore = loadHighScore();

    // Titre avec effet de glow
    Text title(font, "NEON RACING", 64);
    title.setFillColor(NEON_CYAN);
    title.setPosition(Vector2f(180.f, 80.f));
    title.setStyle(Text::Bold);

    // Sous-titre avec animation
    Text subtitle(font, "High Speed , High Stakes , High Score", 20);
    subtitle.setFillColor(NEON_GREEN);
    subtitle.setPosition(Vector2f(180.f, 160.f));

    // Score avec style
    Text highScoreText(font, "RECORD: " + std::to_string(highScore), 28);
    highScoreText.setFillColor(GOLD);
    highScoreText.setPosition(Vector2f(280.f, 200.f));
    highScoreText.setStyle(Text::Bold);

    // Boutons stylisés
    StyledButton startButton(font, ">> COMMENCER <<", Vector2f(300.f, 280.f), Vector2f(200.f, 60.f));
    StyledButton quitButton(font, ">> QUITTER <<", Vector2f(300.f, 360.f), Vector2f(200.f, 60.f));

    Clock animationClock;

    while (window.isOpen()) {
        float time = animationClock.getElapsedTime().asSeconds();
        Vector2f mousePos = static_cast<Vector2f>(Mouse::getPosition(window));

        while (std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }

            if (event->is<Event::MouseButtonPressed>()) {
                const auto& mouseButton = event->getIf<Event::MouseButtonPressed>();
                if (mouseButton->button == Mouse::Button::Left) {
                    Vector2f clickPos = window.mapPixelToCoords(Vector2i(mouseButton->position.x, mouseButton->position.y));

                    if (startButton.isClicked(clickPos)) {
                        return true;
                    } else if (quitButton.isClicked(clickPos)) {
                        window.close();
                        return false;
                    }
                }
            }
        }

        // Titre statique
        title.setFillColor(NEON_CYAN);

        // Mise à jour des boutons
        startButton.update(mousePos);
        quitButton.update(mousePos);

        // Mise à jour du champ d'étoiles
        starField.update(0.016f); // ~60 FPS

        // Rendu
        window.clear(DARK_BLUE);
        
        // Fond dégradé simulé
        for (int i = 0; i < 600; i += 10) {
            RectangleShape gradientLine(Vector2f(800, 10));
            gradientLine.setPosition(Vector2f(0, i));
            unsigned char intensity = static_cast<unsigned char>(255 * (1.0f - i / 600.0f) * 0.1f);
            gradientLine.setFillColor(Color(intensity, intensity, intensity * 2, 50));
            window.draw(gradientLine);
        }

        starField.draw(window);
        
        window.draw(title);
        window.draw(subtitle);
        window.draw(highScoreText);
        startButton.draw(window);
        quitButton.draw(window);
        window.display();
    }

    return false;
}

bool showGameOver(RenderWindow& window, int currentScore, int highScore, bool isNewRecord) {
    Font font;
    if (!font.openFromFile("assets/fonts/arial.ttf")) {
        return false;
    }

    StarField starField(30, 600);
    Clock animationClock;

    // Textes stylisés
    Text gameOverText(font, "*** GAME OVER ***", 56);
    gameOverText.setFillColor(NEON_PINK);
    gameOverText.setPosition(Vector2f(120.f, 80.f));
    gameOverText.setStyle(Text::Bold);

    Text scoreText(font, "SCORE: " + std::to_string(currentScore), 36);
    scoreText.setFillColor(NEON_CYAN);
    scoreText.setPosition(Vector2f(280.f, 160.f));
    scoreText.setStyle(Text::Bold);

    Text highScoreText(font, "RECORD: " + std::to_string(highScore), 28);
    highScoreText.setFillColor(GOLD);
    highScoreText.setPosition(Vector2f(250.f, 200.f));

    Text newRecordText(font, "*** NOUVEAU RECORD! ***", 32); 
    newRecordText.setFillColor(NEON_GREEN);
    newRecordText.setPosition(Vector2f(150.f, 240.f));
    newRecordText.setStyle(Text::Bold);

    // Boutons stylisés
    float buttonY = isNewRecord ? 320.f : 280.f;
    StyledButton retryButton(font, ">> REJOUER <<", Vector2f(300.f, buttonY), Vector2f(200.f, 60.f));
    StyledButton quitButton(font, ">> QUITTER <<", Vector2f(300.f, buttonY + 80.f), Vector2f(200.f, 60.f));

    while (window.isOpen()) {
        float time = animationClock.getElapsedTime().asSeconds();
        Vector2f mousePos = static_cast<Vector2f>(Mouse::getPosition(window));

        while (std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
                return false;
            }

            if (event->is<Event::MouseButtonPressed>()) {
                const auto& mouseButton = event->getIf<Event::MouseButtonPressed>();
                if (mouseButton->button == Mouse::Button::Left) {
                    Vector2f clickPos = window.mapPixelToCoords(Vector2i(mouseButton->position.x, mouseButton->position.y));

                    if (retryButton.isClicked(clickPos)) {
                        return true;
                    } else if (quitButton.isClicked(clickPos)) {
                        window.close();
                        return false;
                    }
                }
            }
        }

        // Animation du nouveau record
        if (isNewRecord) {
            unsigned char alpha = static_cast<unsigned char>(128 + 127 * sin(time * 4));
            Color recordColor = NEON_GREEN;
            recordColor.a = alpha;
            newRecordText.setFillColor(recordColor);
        }

        // Mise à jour
        retryButton.update(mousePos);
        quitButton.update(mousePos);
        starField.update(0.016f);

        // Rendu
        window.clear(DARK_BLUE);
        starField.draw(window);
        
        window.draw(gameOverText);
        window.draw(scoreText);
        window.draw(highScoreText);
        if (isNewRecord) {
            window.draw(newRecordText);
        }
        retryButton.draw(window);
        quitButton.draw(window);
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

    // Effet d'écran qui tremble
    Clock shakeClock;
    Clock waitClock;
    
    while (waitClock.getElapsedTime().asSeconds() < 2.0f) {
        while (std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        // Effet de secousse
        float shakeIntensity = 10.0f * (2.0f - waitClock.getElapsedTime().asSeconds());
        Vector2f shake(
            (rand() % 21 - 10) * shakeIntensity / 10.0f,
            (rand() % 21 - 10) * shakeIntensity / 10.0f
        );

        window.clear(Color::Red);
        explosionSprite.setPosition(position + shake);
        window.draw(explosionSprite);
        window.display();
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
    
    // Variables pour le score
    int score = 0;
    float scoreTimer = 0.f;
    int highScore = loadHighScore();

    RenderWindow window(VideoMode({windowWidth, windowHeight}), "Neon Racing");
    window.setFramerateLimit(60);

    if (!showMenu(window)) {
        return 0;
    }

    sf::Clock clock;
    StarField starField(30, 600);

    // Route avec dégradé
    RectangleShape road(Vector2f(static_cast<float>(roadWidth), static_cast<float>(windowHeight)));
    road.setFillColor(Color(20, 20, 30));
    road.setPosition(Vector2f(0.f, 0.f));

    // Panneau latéral moderne
    RectangleShape menu(Vector2f(static_cast<float>(windowWidth - roadWidth), static_cast<float>(windowHeight)));
    menu.setFillColor(DARK_BLUE);
    menu.setPosition(Vector2f(static_cast<float>(roadWidth), 0.f));

    // Bordures de la route avec glow
    RectangleShape leftBorder(Vector2f(4.f, static_cast<float>(windowHeight)));
    leftBorder.setFillColor(NEON_CYAN);
    leftBorder.setPosition(Vector2f(-2.f, 0.f));

    RectangleShape rightBorder(Vector2f(4.f, static_cast<float>(windowHeight)));
    rightBorder.setFillColor(NEON_CYAN);
    rightBorder.setPosition(Vector2f(static_cast<float>(roadWidth) - 2.f, 0.f));

    // Lignes de voie avec couleur blanche statique
    std::vector<std::vector<RectangleShape>> dashedLaneLines;
    const float dashLength = 40.f;
    const float gapLength = 20.f;
    const float lineWidth = 6.f;

    for (int i = 1; i < laneCount; ++i) {
        std::vector<RectangleShape> dashesForThisLane;
        float currentY = 0.f;

        while (currentY < windowHeight) {
            RectangleShape dash(Vector2f(lineWidth, dashLength));
            dash.setFillColor(Color::White); // Couleur blanche statique
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

    // Interface utilisateur stylisée
    Text levelText(font, "NIVEAU: 1", 22);
    Text jumpText(font, "SAUTS: 5", 22);
    Text scoreText(font, "SCORE: 0", 26);
    Text highScoreText(font, "RECORD: " + std::to_string(highScore), 20);

    levelText.setPosition(Vector2f(static_cast<float>(roadWidth) + 20.f, 30.f));
    levelText.setFillColor(NEON_CYAN);
    levelText.setStyle(Text::Bold);

    jumpText.setPosition(Vector2f(static_cast<float>(roadWidth) + 20.f, 120.f));
    jumpText.setFillColor(NEON_GREEN);
    jumpText.setStyle(Text::Bold);

    scoreText.setPosition(Vector2f(static_cast<float>(roadWidth) + 20.f, 75.f));
    scoreText.setFillColor(GOLD);
    scoreText.setStyle(Text::Bold);

    highScoreText.setPosition(Vector2f(static_cast<float>(roadWidth) + 20.f, 160.f));
    highScoreText.setFillColor(SILVER);

    // Panneaux d'information avec bordures
    RectangleShape infoPanelBorder(Vector2f(260.f, 140.f));
    infoPanelBorder.setPosition(Vector2f(static_cast<float>(roadWidth) + 10.f, 20.f));
    infoPanelBorder.setFillColor(Color::Transparent);
    infoPanelBorder.setOutlineThickness(2.0f);
    infoPanelBorder.setOutlineColor(NEON_CYAN);

    Clock animationClock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        float time = animationClock.getElapsedTime().asSeconds();

        while (std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        scoreTimer += deltaTime;
        if (scoreTimer >= 0.1f) {
            score += 1;
            scoreTimer = 0.f;
            scoreText.setString("SCORE: " + std::to_string(score));
        }

        levelTimer += deltaTime;
        if (levelTimer >= levelDuration) {
            level++;
            levelTimer = 0.f;
            enemySpeed += 0.5f;
            player.resetJumpCount();
            score += 100 * level;

            levelText.setString("NIVEAU: " + std::to_string(level));
            
            std::cout << "Niveau atteint : " << level << ", Vitesse ennemie: " << enemySpeed << std::endl;
        }

        // Animation du mouvement des lignes de voie (sans effet de pulsation)
        float dashSpeed = 3.0f + level * 0.5f;
        for (auto& laneOfDashes : dashedLaneLines) {
            for (auto& dash : laneOfDashes) {
                dash.move(Vector2f(0.f, dashSpeed));
                if (dash.getPosition().y > windowHeight) {
                    dash.setPosition(Vector2f(dash.getPosition().x, -dash.getSize().y));
                }
                // Couleur blanche statique - pas d'animation de pulsation
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
                if (e.getBounds().position.y > windowHeight) {
                    score += 10;
                    return true;
                }
                return false;
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
            std::cout << "Collision detectee ! Score final: " << score << std::endl;

            showExplosion(window, player.getBounds().position);

            bool isNewRecord = false;
            if (score > highScore) {
                highScore = score;
                saveHighScore(highScore);
                isNewRecord = true;
                std::cout << "NOUVEAU RECORD: " << highScore << std::endl;
            }

            bool retry = showGameOver(window, score, highScore, isNewRecord);
            if (retry) {
                main();
            } else {
                window.close();
            }
            return 0;
        }

        player.handleInput();
        player.update();

        // Mise à jour du champ d'étoiles
        starField.update(deltaTime);

        // Rendu
        window.clear(Color::Black);
        
        // Fond avec étoiles
        starField.draw(window);
        
        // Route avec dégradé
        window.draw(road);
        window.draw(leftBorder);
        window.draw(rightBorder);

        // Lignes de voie blanches sans animation
        for (const auto& laneOfDashes : dashedLaneLines) {
            for (const auto& dash : laneOfDashes) {
                window.draw(dash);
            }
        }

        player.draw(window);

        for (auto& enemy : enemies)
            enemy.draw(window);

        jumpText.setString("SAUTS: " + std::to_string(player.getJumpCount()));

        // Interface utilisateur
        window.draw(menu);
        window.draw(infoPanelBorder);
        window.draw(jumpText);
        window.draw(levelText);
        window.draw(scoreText);
        window.draw(highScoreText);
        
        window.display();
    }

    return 0;
}