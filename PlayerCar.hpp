#pragma once
#include <SFML/Graphics.hpp>

class PlayerCar {
public:
    PlayerCar(const sf::Texture& texture, int laneCount, float laneWidth, float roadHeight);

    void handleInput();
    void update();
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    bool isJumping() const;


private:
    sf::Sprite carSprite;
    int currentLane;
    float carSpeed;
    float jumpOffset;
    bool isJumpingFlag;
    float jumpVelocity;
    float baseY;

    int laneCount;
    float laneWidth;
    float roadHeight;

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void jump();
};
