#pragma once
#include <glm/glm.hpp>

class Car {
public:
    Car();

    // Update car state per timer event
    void update(float deltaTime);

    // Getters for transformations
    glm::vec3 getPosition() const;
    float getHeading() const;
    float getWheelRotation() const;

    // methods to move car forward, turn, etc.
    void accelerate(float amount);
    void turn(float angle);

private:
    glm::vec3 m_position;       // Car position in world space
    float m_heading;            // Car heading angle (radians)
    float m_wheelRotation;      // Current rotation angle of wheels (for spinning animation)
    float m_wheelSpinSpeed;     // Radians per second for wheel spin
};
