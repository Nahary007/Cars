#include "EnemyCar.hpp"

EnemyCar::EnemyCar(const sf::Texture& texture, float laneX, float speed)
    : sprite(texture), speed(speed) {
    sprite.setScale(sf::Vector2f(0.5f, 0.5f));
    sprite.setPosition(sf::Vector2f(laneX, -sprite.getGlobalBounds().size.y));
}

void EnemyCar::update() {
    sprite.move(sf::Vector2f(0.f, speed));
}

void EnemyCar::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect EnemyCar::getBounds() const {
    return sprite.getGlobalBounds();
}
