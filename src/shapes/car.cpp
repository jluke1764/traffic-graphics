

#include "shapes/car.h"
#include "glm/ext/matrix_transform.hpp"

Car::Car() {

    std::cout << "brand new car" << std::endl;

    // "type": "cube",
    // "ambient": [0.5, 0, 0],
    // "diffuse": [0.7, 0, 0],
    // "specular": [1.0, 1.0, 1.0],
    // "shininess": 15.0

    SceneMaterial material = SceneMaterial{.cAmbient = {0.5, 0.0, 0.0, 1},
                                           .cDiffuse = {0.7, 0.0, 0.0, 1},
                                           .cSpecular = {1.0, 1.0, 1.0, 1},
                                           .shininess = 15.0};

    ScenePrimitive rectangle = ScenePrimitive{.type = PrimitiveType::PRIMITIVE_CUBE,
                                              .material = material};

    //car space
    glm::mat4 bodyCTM = glm::scale(glm::mat4(1.0), {.2, .1, .1});

    m_body = RenderShapeData{.primitive = rectangle,
                             .ctm = bodyCTM};

    m_position = glm::vec3(0.0);
    // m_ctm = glm::translate(glm::rotate(glm::mat4(1.0), glm::radians(m_angleFacing), glm::vec3(0, 1, 0)), m_position);

}

std::vector<RenderShapeData> Car::getShapeData() {
    std::vector<RenderShapeData> ret;
    ret.clear();
    //world space

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0), m_angleFacing, glm::vec3(0, 1, 0));
    glm::mat4 translation = glm::translate(glm::mat4(1.0), m_position);

    // printMatrix4x4(m_body.ctm);

    ret.push_back(RenderShapeData{.primitive = m_body.primitive, .ctm = translation*rotation*m_body.ctm});

    return ret;
}

void Car::setVelocity(float v) {
    m_speed = v;
}

void Car::goForward() {
    std::cout << "go forward" << std::endl;

    glm::vec3 dir = glm::vec3(cos(m_angleFacing), 0, sin(m_angleFacing));
    dir = glm::normalize(dir);
    m_position = m_position + m_speed*dir;

    printPosition();


}

void Car::turnRight() {
    m_angleFacing += -90;
}


// https://msl.cs.uiuc.edu/planning/node658.html
void Car::drive(float steeringAngle, float speed) {

    if (steeringAngle > M_PI/2) {
        std::cout << "max steering angle exceeded" << std::endl;
    }

    if (speed > 1) {
        std::cout << "max speed exceeded" << std::endl;
    }

    float dtheta = speed/m_wheelbase * tan(steeringAngle);
    m_angleFacing = m_angleFacing + dtheta;

    float dx = speed*cos(m_angleFacing);
    float dz = -1*speed*sin(m_angleFacing);

    m_position = glm::vec3(m_position.x + dx, 0, m_position.z + dz);

    printPosition();

}

void Car::printPosition() {
    std::cout << "pos: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
    std::cout << "angleFacing: " << glm::degrees(m_angleFacing) << std::endl;

}


