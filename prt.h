#pragma once
#include <glm/glm.hpp>

class Particle {
private:
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec2 acceleration;
    float radius;
    float restitution;

public:
    Particle(glm::vec2 startPosition, float startRadius, float rest = 0.95f);
    void update(float dt, float floorY);
    glm::vec2 getPosition() const;
    float getRadius() const;
};