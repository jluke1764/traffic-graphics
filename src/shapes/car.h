
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

    std::vector<RenderShapeData> getShapeData();
    void goForward();
    void turnRight();
    void setVelocity(float v);


    void printMatrix4x4(glm::mat4 M) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                std::cout << M[j][i] << ", ";
            }

            std::cout << std::endl;
        }
    }

    void printPosition();


private:
    glm::vec3 m_position = glm::vec3(0.0);
    glm::vec3 m_prev_position = glm::vec3(0.0);
    float m_angleFacing = 0.0;
    float m_prev_angleFacing = 0.0;

    // glm::mat4 m_ctm;
    // RenderShapeData m_wheel;
    RenderShapeData m_body;

    float m_velocity = 0.0;
    float m_acceleration = 0.0;


};
