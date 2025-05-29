#ifndef ENEMYCAR_HPP
#define ENEMYCAR_HPP

#include <SFML/Graphics.hpp>

class EnemyCar {
private:
    sf::Sprite sprite;
    float speed;

public:
    EnemyCar(const sf::Texture& texture, float laneX, float speed);

    void update();
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
};

#endif
