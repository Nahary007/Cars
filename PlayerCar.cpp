#include "PlayerCar.hpp"
using namespace sf;

PlayerCar::PlayerCar(const Texture& texture, int laneCount, float laneWidth, float roadHeight)
    : carSprite(texture), currentLane(1), carSpeed(5.f), jumpOffset(0.f), isJumping(false), jumpVelocity(0.f),
      laneCount(laneCount), laneWidth(laneWidth), roadHeight(roadHeight)
    {
        carSprite.setScale(Vector2f(0.5f, 0.5f));
        float carWidth = carSprite.getGlobalBounds().size.x;
        float x = currentLane * laneWidth + (laneWidth - carWidth) / 2.f;
        float y = roadHeight - carSprite.getGlobalBounds().size.y - 30.f;
        carSprite.setPosition(Vector2f(x, y));
    }


void PlayerCar::handleInput() {
    // Lane switching (left/right movement)
    if (Keyboard::isKeyPressed(Keyboard::Key::Left) || Keyboard::isKeyPressed(Keyboard::Key::A)) {
        moveLeft();
    }
    if (Keyboard::isKeyPressed(Keyboard::Key::Right) || Keyboard::isKeyPressed(Keyboard::Key::D)) {
        moveRight();
    }
    
    // Forward/backward movement
    if (Keyboard::isKeyPressed(Keyboard::Key::Up) || Keyboard::isKeyPressed(Keyboard::Key::W)) {
        moveUp();
    }
    if (Keyboard::isKeyPressed(Keyboard::Key::Down) || Keyboard::isKeyPressed(Keyboard::Key::S)) {
        moveDown();
    }
    
    // Jump
    if (Keyboard::isKeyPressed(Keyboard::Key::Space)) {
        jump();
    }
}

void PlayerCar::update() {
    
    if (isJumping) {
        jumpVelocity += 0.5f;
        jumpOffset += jumpVelocity;

        if (jumpOffset >= 0) {
            jumpOffset = 0;
            isJumping = false;
            jumpVelocity = 0;
        }

        auto pos = carSprite.getPosition();
        carSprite.setPosition(Vector2f(pos.x, roadHeight - carSprite.getGlobalBounds().size.y - 30.f + jumpOffset));
    }
}

void PlayerCar::draw(sf::RenderWindow& window) {
    window.draw(carSprite);
}

void PlayerCar::moveLeft() {
    if (currentLane > 0) {
        currentLane--;
        float x = currentLane * laneWidth + (laneWidth - carSprite.getGlobalBounds().size.x) / 2.f;
        carSprite.setPosition(Vector2f(x, carSprite.getPosition().y));
    }
}

void PlayerCar::moveRight() {
    if (currentLane < laneCount - 1) {
        currentLane++;
        float x = currentLane * laneWidth + (laneWidth - carSprite.getGlobalBounds().size.x) / 2.f;
        carSprite.setPosition(Vector2f(x, carSprite.getPosition().y));
    }
}

void PlayerCar::moveUp() {
    carSprite.move(Vector2f(0.f, - carSpeed));
}

void PlayerCar::moveDown() {
    carSprite.move(Vector2f(0.f, carSpeed));
}

void PlayerCar::jump() {
    if (!isJumping) {
        isJumping = true;
        jumpVelocity = -10.f;
    }
}
