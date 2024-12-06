#pragma once
#include <glm/glm.hpp>

class Car {
public:
    Car();

    // Update car state per frame
    void update(float deltaTime);

    glm::vec3 getPosition() const;
    float getHeading() const;
    float getWheelRotation() const;

    void setPosition(const glm::vec3 &pos);
    void setHeading(float heading);
    void setWheelSpinSpeed(float speed);

private:
    glm::vec3 m_position;
    float m_heading;       // radians
    float m_wheelRotation; // radians
    float m_wheelSpinSpeed; // radians per second
};
