#include "PlayerCar.hpp"
using namespace sf;

PlayerCar::PlayerCar(const Texture& texture, int laneCount, float laneWidth, float roadHeight)
    : carSprite(texture), currentLane(1), carSpeed(5.f), jumpOffset(0.f), isJumpingFlag(false), jumpVelocity(0.f),
      laneCount(laneCount), laneWidth(laneWidth), roadHeight(roadHeight)
    {
        carSprite.setScale(Vector2f(0.5f, 0.5f));
        float carWidth = carSprite.getGlobalBounds().size.x;
        float x = currentLane * laneWidth + (laneWidth - carWidth) / 2.f;
        float y = roadHeight - carSprite.getGlobalBounds().size.y - 30.f;
        carSprite.setPosition(Vector2f(x, y));
        baseY = y;
    }

void PlayerCar::handleInput() {
    // Variables statiques pour éviter les mouvements répétés
    static bool leftPressed = false;
    static bool rightPressed = false;
    static bool spacePressed = false;
    
    // Lane switching (left/right movement) - une seule fois par pression
    bool leftCurrentlyPressed = Keyboard::isKeyPressed(Keyboard::Key::Left) || Keyboard::isKeyPressed(Keyboard::Key::A);
    bool rightCurrentlyPressed = Keyboard::isKeyPressed(Keyboard::Key::Right) || Keyboard::isKeyPressed(Keyboard::Key::D);
    bool spaceCurrentlyPressed = Keyboard::isKeyPressed(Keyboard::Key::Space);
    
    if (leftCurrentlyPressed && !leftPressed) {
        moveLeft();
    }
    if (rightCurrentlyPressed && !rightPressed) {
        moveRight();
    }
    
    // Forward/backward movement (continu)
    if (Keyboard::isKeyPressed(Keyboard::Key::Up) || Keyboard::isKeyPressed(Keyboard::Key::W)) {
        moveUp();
    }
    if (Keyboard::isKeyPressed(Keyboard::Key::Down) || Keyboard::isKeyPressed(Keyboard::Key::S)) {
        moveDown();
    }
    
    // Jump - une seule fois par pression
    if (spaceCurrentlyPressed && !spacePressed) {
        if (!isJumpingFlag && jumpCount > 0) {
            jump();              
            decrementJumpCount();
        }
    }
    
    // Mettre à jour l'état des touches
    leftPressed = leftCurrentlyPressed;
    rightPressed = rightCurrentlyPressed;
    spacePressed = spaceCurrentlyPressed;
}

void PlayerCar::update() {
    if (isJumpingFlag) {
        jumpVelocity += 0.5f;
        jumpOffset += jumpVelocity;

        if (jumpOffset >= 0) {
            jumpOffset = 0;
            isJumpingFlag = false;
            jumpVelocity = 0;
        }


        carSprite.setPosition(Vector2f(carSprite.getPosition().x, baseY + jumpOffset));
    } else {

        baseY = carSprite.getPosition().y;
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
    float newY = carSprite.getPosition().y - carSpeed;
    if (newY >= 0) {
        carSprite.setPosition(Vector2f(carSprite.getPosition().x, newY));
        if (!isJumping()) {
            baseY = newY;
        }
    }
}

void PlayerCar::moveDown() {
    float newY = carSprite.getPosition().y + carSpeed;
    float carHeight = carSprite.getGlobalBounds().size.y;

    if (newY + carHeight <= roadHeight) {
        carSprite.setPosition(Vector2f(carSprite.getPosition().x, newY));
        if (!isJumping()) {
            baseY = newY;
        }
    }
}

void PlayerCar::jump() {
    if (!isJumpingFlag) {
        isJumpingFlag = true;
        jumpVelocity = -10.f;
    }
}

FloatRect PlayerCar::getBounds() const {
    return carSprite.getGlobalBounds();
}

bool PlayerCar::isJumping() const {
    return isJumpingFlag;
}

int PlayerCar::getJumpCount() const {
    return jumpCount;
}

void PlayerCar::resetJumpCount() {
    jumpCount = 3; // Tu peux adapter ce nombre
}

void PlayerCar::decrementJumpCount() {
    if (jumpCount > 0)
        jumpCount--;
}