#include "car.h"
#include <glm/gtc/constants.hpp>
#include <cmath>

Car::Car() {
    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_heading = 0.0f;
    m_wheelRotation = 0.0f;
    m_wheelSpinSpeed = glm::radians(180.0f); // wheels spin at 180 deg/sec for example
}

void Car::update(float deltaTime) {
    m_wheelRotation += m_wheelSpinSpeed * deltaTime;
    if (m_wheelRotation > glm::two_pi<float>()) {
        m_wheelRotation = std::fmod(m_wheelRotation, glm::two_pi<float>());
    }
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

void Car::setPosition(const glm::vec3 &pos) {
    m_position = pos;
}

void Car::setHeading(float heading) {
    m_heading = heading;
}

void Car::setWheelSpinSpeed(float speed) {
    m_wheelSpinSpeed = speed;
}
