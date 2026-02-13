#include "prt.h"
#include "const.h"

Particle::Particle(glm::vec2 startPosition, float startRadius, float restitution)
    : position(startPosition), velocity(glm::vec2(0.0f, 0.0f)), acceleration(0.0f, egrav),
    radius(startRadius), restitution(0.95f) {
}

void Particle::update(float dt, float floorY) {  // dt --> fixedDeltaTime
    glm::vec2 initialPos = position;
    glm::vec2 initialVel = velocity;

    // Full tentative integration
    velocity = initialVel + acceleration * dt;
    position = initialPos + initialVel * dt + 0.5f * acceleration * dt * dt;

    // Check floor collision
    if (position.y - radius < floorY) {
        // Quadratic equation: 0.5*a*t² + v?*t + (y? - radius - floorY) = 0
        float a = 0.5f * acceleration.y;
        float b = initialVel.y;
        float c = initialPos.y - radius - floorY;

        float discriminant = b * b - 4 * a * c;
        float t_hit = dt;

        if (discriminant >= 0) {
            t_hit = (-b - sqrt(discriminant)) / (2 * a); // First collision time
            t_hit = fmax(0.0f, fmin(t_hit, dt));  // I don't know what this line does 
        }

        // Update to collision point
        position = initialPos + initialVel * t_hit + 0.5f * acceleration * t_hit * t_hit;
        velocity = initialVel + acceleration * t_hit;

        // Apply bounce with restitution
        velocity.y = -velocity.y * restitution;

        // Apply remaining time after bounce
        float remaining = dt - t_hit;
        position += velocity * remaining + 0.5f * acceleration * remaining * remaining;
        velocity += acceleration * remaining;

        // Correct any residual penetration
        if (position.y - radius < floorY) {
            position.y = floorY + radius;
            velocity.y = 0; // Prevent multiple bounces
        }
    }
}

glm::vec2 Particle::getPosition() const {
    return position;
}

float Particle::getRadius() const {
    return radius;
}



