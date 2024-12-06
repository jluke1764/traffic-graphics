#include "car.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
#include <cmath>
#include "parser/scenedata.h"
#include "parser/sceneparser.h"
#include <iostream>

Car::Car()
    : m_position(0.0f,0.0f,0.0f),
    m_heading(0.0f),
    m_wheelRotation(0.0f),
    m_wheelSpinSpeed(glm::radians(90.0f)) // can change spin speed
{}

void Car::update(float deltaTime) {
    // Update wheel rotation
    m_wheelRotation += m_wheelSpinSpeed * deltaTime;
    if (m_wheelRotation > glm::two_pi<float>()) {
        m_wheelRotation = fmod(m_wheelRotation, glm::two_pi<float>());
    }

    glm::vec3 direction(glm::sin(m_heading), 0.0f, glm::cos(m_heading));
    m_position += direction * 0.1f * deltaTime;
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

glm::vec3 Car::getPosition() const {
    return m_position;
}

float Car::getHeading() const {
    return m_heading;
}

float Car::getWheelRotation() const {
    return m_wheelRotation;
}

void Car::addBodyShape(RenderShapeData *bodyShape) {
    m_body.push_back(bodyShape);
}

void Car::addWheelShape(RenderShapeData *wheelShape) {
    m_wheels.push_back(wheelShape);
}

void Car::applyTransformsToShapes() {
    std::cout << m_position[0] << " " << m_position[1] << " " << m_position[2] << std::endl;
    glm::mat4 carTransform = glm::translate(glm::mat4(1.0f), m_position)
                             * glm::rotate(glm::mat4(1.0f), m_heading, glm::vec3(0,1,0));

    // Wheel rotation, currently x axis?
    glm::mat4 wheelRotation = glm::rotate(glm::mat4(1.0f), m_wheelRotation, glm::vec3(1,0,0));

    // Update body shapes
    for (auto body : m_body) {
        glm::mat4 originalBodyTransform = body->ctm;
        body->ctm = carTransform * originalBodyTransform;
    }

    // Update wheel shapes
    for (auto wheel : m_wheels) {
        glm::mat4 originalWheelTransform = wheel->ctm;
        wheel->ctm = carTransform * originalWheelTransform;
    }
}
