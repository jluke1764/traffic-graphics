
#pragma once

#include "parser/sceneparser.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include <iostream>
#include <ostream>
#include <vector>
#include <glm/glm.hpp>

class Car
{
public:
    Car();
    Car(glm::vec4 color, glm::vec3 startingPosition, float startingDirectionAngle);

    std::vector<RenderShapeData> getShapeData();
    void goForward();
    void turnRight();
    void setVelocity(float v);
    void drive(float steeringAngle, float speed);


    void printMatrix4x4(glm::mat4 M) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                std::cout << M[j][i] << ", ";
            }

            std::cout << std::endl;
        }
    }

    void printPosition();

    void update(int time);

    void setDesiredPosition(glm::vec3 desiredPos);
    glm::vec3 getPosition();



private:
    glm::vec3 m_position = glm::vec3(0.0);
    float m_directionAngle = 0.0;

    float m_speed = 0.5; //position change in one timestep
    float m_acceleration = 0.0;

    float m_wheelbase = 2.5;
    float m_wheelRadius = 0.5*0.7; //based on scaling

    float m_steeringAngle = 0.0;
    float m_steeringSpeed = 1.0; //max angle change in one timestep
    float m_maxSteeringAngle = glm::radians(30.0f);



    // glm::mat4 m_ctm;
    RenderShapeData m_body;
    glm::mat4 m_translateBody;
    glm::mat4 m_scaleBody;

    RenderShapeData m_wheelFL;
    glm::mat4 m_translateFL;
    RenderShapeData m_wheelFR;
    glm::mat4 m_translateFR;
    RenderShapeData m_wheelBL;
    glm::mat4 m_translateBL;
    RenderShapeData m_wheelBR;
    glm::mat4 m_translateBR;
    glm::mat4 m_scaleWheel;
    glm::mat4 m_rotateWheelLeft;
    glm::mat4 m_rotateWheelRight;

    glm::vec3 m_desiredPosition = glm::vec3(0.0);











};
