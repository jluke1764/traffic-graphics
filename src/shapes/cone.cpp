#include "cone.h"
#include <iostream>

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}
glm::vec2 Cone::getUV(glm::vec3 point) {
    float u;
    float v;
    float x = point.x;
    float y = point.y;
    float z = point.z;
    if (y==-0.5) { //base
        u = x+0.5;
        v = z+0.5;
    } else {
        float theta = atan2(z,x);
        // std::cout << "theta: " << theta << std::endl;
        if (theta < 0) {
            u = -theta/(2*M_PI);
        } else if (theta > 0) {
            u = 1-theta/(2*M_PI);
        }
        v = y+0.5;
    }
    return glm::vec2(u,v);
}


void Cone::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, bool isTop, bool isBottom, bool isSide) {
    glm::vec3 normal;
    glm::vec3 normalTL;
    glm::vec3 normalTR;
    glm::vec3 normalBL;
    glm::vec3 normalBR;
    if (isBottom) {
        normal = glm::vec3(0, -1, 0);
    } else if (isSide || isTop) {
        normalTL = glm::normalize(glm::vec3(2*topLeft.x, (0.5-topLeft.y)/2, 2*topLeft.z));
        normalTR = glm::normalize(glm::vec3(2*topRight.x, (0.5-topRight.y)/2, 2*topRight.z));
        normalBL = glm::normalize(glm::vec3(2*bottomLeft.x, (0.5-bottomLeft.y)/2, 2*bottomLeft.z));
        normalBR = glm::normalize(glm::vec3(2*bottomRight.x, (0.5-bottomRight.y)/2, 2*bottomRight.z));

        if (isTop) {
            normal = glm::normalize(glm::vec3((bottomLeft.x+bottomRight.x)/2,0, (bottomLeft.z+bottomRight.z)/2));
            normal.y = topLeft.y;
            normal = glm::normalize(normal);
            //average x and z only then add in y and normalize
        }
    }
    if (isSide) {
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normalTL);
        insertVec2(m_vertexData, getUV(topLeft));

        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normalTR);
        insertVec2(m_vertexData, getUV(topRight));

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normalBL);
        insertVec2(m_vertexData, getUV(bottomLeft));


        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normalBL);
        insertVec2(m_vertexData, getUV(bottomLeft));

        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normalTR);
        insertVec2(m_vertexData, getUV(topRight));

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalBR);
        insertVec2(m_vertexData, getUV(bottomRight));
    }
    if (isBottom) {
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(topLeft));

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(bottomRight));

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(bottomLeft));


        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(topLeft));

        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(topRight));

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(bottomRight));

    } else if (isTop) {
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(topLeft));

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalBR);
        insertVec2(m_vertexData, getUV(bottomRight));

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normalBL);
        insertVec2(m_vertexData, getUV(bottomLeft));
    }
}


void Cone::makeSide() {
    float baseRadius = 0.5;
    float height = 1.f;
    float thetaStep = 2 * M_PI / m_param2;
    float yStep = 1.0f / m_param1;  // The cylinder's height is 1.0, so divide it equally

    for (int i = 0; i < m_param2; ++i) {
        float theta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;

        for (int j = 0; j < m_param1; ++j) {
            float yTop = baseRadius - j * yStep;
            float yBottom = baseRadius - (j + 1) * yStep;

            float topRadius = baseRadius*(baseRadius-yTop);
            float bottomRadius = baseRadius*(baseRadius-yBottom);

            glm::vec3 topLeft(topRadius * cos(theta), yTop, topRadius * sin(theta));
            glm::vec3 topRight(topRadius * cos(nextTheta), yTop, topRadius * sin(nextTheta));
            glm::vec3 bottomLeft(bottomRadius * cos(theta), yBottom, bottomRadius * sin(theta));
            glm::vec3 bottomRight(bottomRadius * cos(nextTheta), yBottom, bottomRadius * sin(nextTheta));

            if (j==0) {
                makeTile(topLeft, topRight, bottomLeft, bottomRight, true, false, false);
            } else {
                makeTile(topLeft, topRight, bottomLeft, bottomRight, false, false, true);
            }
        }
    }
}

void Cone::makeCap() {
    float radius = 0.5f;
    float y = -0.5f;
    glm::vec3 center(0.0f, y, 0.0f);
    glm::vec3 normal = glm::vec3(0, -1, 0);
    float thetaStep = 2 * M_PI / m_param2;
    float radiusDiv = radius/m_param1;

    for (int i = 0; i < m_param2; ++i) {
        float theta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        for (int j = 0; j < m_param1; ++j) {
            float distance = radius-j*(radius/m_param1);
            float nextDistance = radius-(j+1)*(radius/m_param1);

            // Corner points of each triangle on the cap
            glm::vec3 p1 = glm::vec3(distance * cos(theta), y, distance * sin(theta));
            glm::vec3 p2 = glm::vec3(nextDistance * cos(nextTheta), y, nextDistance * sin(nextTheta));
            glm::vec3 p3 = glm::vec3(nextDistance * cos(theta), y, nextDistance * sin(theta));

            // distance, distance, nextDistance
            glm::vec3 p4 = glm::vec3(distance * cos(theta), y, distance * sin(theta));
            glm::vec3 p5 = glm::vec3(distance * cos(nextTheta), y, distance * sin(nextTheta));
            glm::vec3 p6 = glm::vec3(nextDistance * cos(nextTheta), y, nextDistance * sin(nextTheta));

            // Insert first vertex
            insertVec3(m_vertexData, p1);
            insertVec3(m_vertexData, normal);
            insertVec2(m_vertexData, getUV(p1));

            // Insert second vertex
            insertVec3(m_vertexData, p2);
            insertVec3(m_vertexData, normal);
            insertVec2(m_vertexData, getUV(p2));

            // Insert third vertex
            insertVec3(m_vertexData, p3);
            insertVec3(m_vertexData, normal);
            insertVec2(m_vertexData, getUV(p3));

            // Insert first vertex
            insertVec3(m_vertexData, p4);
            insertVec3(m_vertexData, normal);
            insertVec2(m_vertexData, getUV(p4));

            // Insert second vertex
            insertVec3(m_vertexData, p5);
            insertVec3(m_vertexData, normal);
            insertVec2(m_vertexData, getUV(p5));

            // Insert third vertex
            insertVec3(m_vertexData, p6);
            insertVec3(m_vertexData, normal);
            insertVec2(m_vertexData, getUV(p6));
        }
    }
}

void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    m_vertexData.clear();
    makeCap();   // Bottom cap
    makeSide();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
void Cone::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}
