

#include "shapes/car.h"
#include "glm/ext/matrix_transform.hpp"

Car::Car(glm::vec4 color, glm::vec3 startingPosition, float startingDirectionAngle) {

    std::cout << "brand new car" << std::endl;

    // "type": "cube",
    // "ambient": [0.5, 0, 0],
    // "diffuse": [0.7, 0, 0],
    // "specular": [1.0, 1.0, 1.0],
    // "shininess": 15.0

    m_position = startingPosition;
    m_directionAngle = startingDirectionAngle;

    SceneMaterial bodyMaterial = SceneMaterial{.cAmbient = {0.3*color.r, 0.3*color.g, 0.3*color.b, 1},
                                           .cDiffuse = {0.7*color.r, 0.7*color.g, 0.7*color.b, 1},
                                           .cSpecular = {1.0, 1.0, 1.0, 1},
                                           .shininess = 15.0};

    ScenePrimitive body = ScenePrimitive{.type = PrimitiveType::PRIMITIVE_CUBE,
                                              .material = bodyMaterial};

    //car space
    m_scaleBody = glm::scale(glm::mat4(1.0), {2.0, 1, 4.0});
    m_translateBody = glm::translate(glm::mat4(1.0), {0, 0.5, 0});

    m_body = RenderShapeData{.primitive = body,
                             .ctm = m_translateBody*m_scaleBody*glm::mat4(1.0)};

    SceneMaterial wheelMaterial = SceneMaterial{.cAmbient = {0.0, 0.0, 0.0, 1},
                                               .cDiffuse = {0.1, 0.1, 0.1, 1},
                                               .cSpecular = {1.0, 1.0, 1.0, 1},
                                               .shininess = 15.0};

    ScenePrimitive wheel = ScenePrimitive{.type = PrimitiveType::PRIMITIVE_CYLINDER,
                                          .material = wheelMaterial};

    m_scaleWheel = glm::scale(glm::mat4(1.0), {0.7, 0.25, 0.7});

    m_rotateWheelLeft = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), {0, 0, 1});
    m_rotateWheelRight = glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), {0, 0, 1});

    glm::mat4 translateLeft = glm::translate(glm::mat4(1.0), {-.95, 0, 0});
    glm::mat4 translateRight = glm::translate(glm::mat4(1.0), {.95, 0, 0});
    glm::mat4 translateBack = glm::translate(glm::mat4(1.0), {0, 0, -1.0});
    glm::mat4 translateFront = glm::translate(glm::mat4(1.0), {0, 0, 1.5});
    glm::mat4 translateHeight = glm::translate(glm::mat4(1.0), {0, 0.35, 0});

    m_translateFL = translateHeight*translateFront*translateLeft;
    m_wheelFL = RenderShapeData{.primitive = wheel,
                                 .ctm = m_translateFL*m_rotateWheelLeft*m_scaleWheel};

    m_translateFR = translateHeight*translateFront*translateRight;
    m_wheelFR = RenderShapeData{.primitive = wheel,
                                 .ctm = m_translateFR*m_rotateWheelRight*m_scaleWheel};

    m_translateBL = translateHeight*translateBack*translateLeft;
    m_wheelBL = RenderShapeData{.primitive = wheel,
                                 .ctm = m_translateBL*m_rotateWheelLeft*m_scaleWheel};

    m_translateBR = translateHeight*translateBack*translateRight;
    m_wheelBR = RenderShapeData{.primitive = wheel,
                                 .ctm = m_translateBR*m_rotateWheelRight*m_scaleWheel};



}

std::vector<RenderShapeData> Car::getShapeData() {
    std::vector<RenderShapeData> ret;
    ret.clear();
    //world space

    glm::mat4 rotateCar = glm::rotate(glm::mat4(1.0), m_directionAngle, glm::vec3(0, 1, 0));
    glm::mat4 translateCar = glm::translate(glm::mat4(1.0), m_position);
    glm::mat4 carCTM = translateCar * rotateCar;

    // printMatrix4x4(m_body.ctm);

    ret.push_back(RenderShapeData{.primitive = m_body.primitive, .ctm = carCTM*m_body.ctm});

    //wheels
    ret.push_back(RenderShapeData{.primitive = m_wheelFL.primitive, .ctm = carCTM*m_wheelFL.ctm});
    ret.push_back(RenderShapeData{.primitive = m_wheelFR.primitive, .ctm = carCTM*m_wheelFR.ctm});
    ret.push_back(RenderShapeData{.primitive = m_wheelBL.primitive, .ctm = carCTM*m_wheelBL.ctm});
    ret.push_back(RenderShapeData{.primitive = m_wheelBR.primitive, .ctm = carCTM*m_wheelBR.ctm});


    return ret;
}

void Car::setVelocity(float v) {
    m_speed = v;
}

void Car::goForward() {
    std::cout << "go forward" << std::endl;

    glm::vec3 dir = glm::vec3(cos(m_directionAngle), 0, sin(m_directionAngle));
    dir = glm::normalize(dir);
    m_position = m_position + m_speed*dir;

    printPosition();


}

void Car::turnRight() {
    m_directionAngle += glm::radians(-90.0);
}


// https://msl.cs.uiuc.edu/planning/node658.html
void Car::drive(float steeringAngleDegrees, float speed) {

    float steeringAngle = glm::radians(steeringAngleDegrees);

    if (steeringAngle > M_PI/2) {
        std::cout << "max steering angle exceeded" << std::endl;
    }

    if (speed > 1) {
        std::cout << "max speed exceeded" << std::endl;
    }

    float dtheta = speed/m_wheelbase * tan(steeringAngle);
    m_directionAngle = m_directionAngle + dtheta;

    float dx = speed*sin(m_directionAngle);
    float dz = speed*cos(m_directionAngle);

    m_position = glm::vec3(m_position.x + dx, 0, m_position.z + dz);

    //visually roll wheels
    float angularVelocity = .01;
    glm::mat4 rollingRotation = glm::rotate(glm::mat4(1.0), angularVelocity, {0, 1, 0});
    //visually steer tires
    glm::mat4 steeringRotation = glm::rotate(glm::mat4(1.0), steeringAngle, {0, 1, 0});

    m_wheelFL.ctm = m_translateFL*steeringRotation*m_rotateWheelLeft*rollingRotation*m_scaleWheel;
    m_wheelFR.ctm = m_translateFR*steeringRotation*m_rotateWheelRight*rollingRotation*m_scaleWheel;
    m_wheelBL.ctm = m_translateBL*m_rotateWheelLeft*rollingRotation*m_scaleWheel;
    m_wheelBR.ctm = m_translateBR*m_rotateWheelLeft*rollingRotation*m_scaleWheel;

    printPosition();

}

void Car::printPosition() {
    std::cout << "pos: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
    std::cout << "angleFacing: " << glm::degrees(m_directionAngle) << std::endl;

}


