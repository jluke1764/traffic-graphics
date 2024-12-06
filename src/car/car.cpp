#include "car.h"
#include <glm/gtc/constants.hpp>

Car::Car() {
    m_position = glm::vec3(0.0f, 0.5f, 0.0f); // Start above ground
    m_heading = 0.0f;
    m_wheelRotation = 0.0f;
    m_wheelSpinSpeed = glm::radians(180.0f); // For example, 180 degrees per second
}

void Car::update(float deltaTime) {
    // Spin the wheels every update
    m_wheelRotation += m_wheelSpinSpeed * deltaTime;

    // Keep angles in range
    if (m_wheelRotation > glm::two_pi<float>()) {
        m_wheelRotation = fmod(m_wheelRotation, glm::two_pi<float>());
    }

    // If you want the car to move or turn automatically, update m_position / m_heading here.
}

glm::vec3 Car::getPosition() const {
    return m_position;
}

float Car::getHeading() const {
    return m_heading;
}

float Car::getWheelRotation() const {
    return m_wheelRotation;
}

void Car::accelerate(float amount) {
    // Move car forward in direction of heading
    float speed = amount; // speed units per second
    m_position += glm::vec3(glm::sin(m_heading), 0.0f, glm::cos(m_heading)) * speed;
}

void Car::turn(float angle) {
    // Adjust heading by angle (radians)
    m_heading += angle;
}
