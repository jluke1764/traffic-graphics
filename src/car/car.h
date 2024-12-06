#pragma once
#include <glm/glm.hpp>
#include <vector>

struct RenderShapeData;

class Car {
public:
    Car();
    void update(float deltaTime);

    void setPosition(const glm::vec3 &pos);
    void setHeading(float heading);
    void setWheelSpinSpeed(float speed);

    glm::vec3 getPosition() const;
    float getHeading() const;
    float getWheelRotation() const;

    void addBodyShape(RenderShapeData *bodyShape);
    void addWheelShape(RenderShapeData *wheelShape);

    void applyTransformsToShapes();

private:
    glm::vec3 m_position;
    float m_heading;
    float m_wheelRotation;
    float m_wheelSpinSpeed;

    std::vector<RenderShapeData*> m_body;
    std::vector<RenderShapeData*> m_wheels;
};
