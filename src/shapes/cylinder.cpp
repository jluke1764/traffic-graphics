#include "cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

glm::vec2 Cylinder::getUV(glm::vec3 point) {
    float u = 0.f;
    float v = 0.f;
    float x = point.x;
    float y = point.y;
    float z = point.z;
    if (y==0.5) { //top face
        u = x+0.5;
        v = -z+0.5;
    } else if (y==-0.5) { //bottom face
        u = x+0.5;
        v = z+0.5;
    } else if ((y>-0.5) and (y<0.5)) { //middle
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


void Cylinder::makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight, bool isTop, bool isBottom, bool isSide) {
    glm::vec3 normal;
    glm::vec3 normalTL;
    glm::vec3 normalTR;
    glm::vec3 normalBL;
    glm::vec3 normalBR;
    if (isTop) {
        normal = glm::vec3(0, 1, 0);
    } else if (isBottom) {
        normal = glm::vec3(0, -1, 0);
    } else if (isSide) {
        normalTL = glm::normalize(glm::vec3(topLeft.x, 0, topLeft.z));
        normalTR = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
        normalBL = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
        normalBR = glm::normalize(glm::vec3(bottomRight.x, 0, bottomRight.z));
    }
    if (isSide) {
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normalTL);
        insertVec2(m_vertexData, getUV(topLeft));

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalBR);
        insertVec2(m_vertexData, getUV(bottomRight));

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normalBL);
        insertVec2(m_vertexData, getUV(bottomLeft));

        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normalTL);
        insertVec2(m_vertexData, getUV(topLeft));

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

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(bottomLeft));

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(bottomRight));

        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(topLeft));

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(bottomRight));

        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, getUV(topRight));
    }
}


void Cylinder::makeSide() {
    float radius = 0.5f;
    float thetaStep = 2 * M_PI / m_param2;
    float yStep = 1.0f / m_param1;  // The cylinder's height is 1.0, so divide it equally

    for (int i = 0; i < m_param2; ++i) {
        float theta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;

        for (int j = 0; j < m_param1; ++j) {
            float yTop = radius - j * yStep;
            float yBottom = radius - (j + 1) * yStep;

            glm::vec3 topLeft(radius * cos(theta), yTop, radius * sin(theta));
            glm::vec3 topRight(radius * cos(nextTheta), yTop, radius * sin(nextTheta));
            glm::vec3 bottomLeft(radius * cos(theta), yBottom, radius * sin(theta));
            glm::vec3 bottomRight(radius * cos(nextTheta), yBottom, radius * sin(nextTheta));

            makeTile(topLeft, topRight, bottomLeft, bottomRight, false, false, true);
        }
    }
}

void Cylinder::makeCap(bool isTop) {
    float radius = 0.5f;
    float y = isTop ? 0.5f : -0.5f;
    glm::vec3 center(0.0f, y, 0.0f);
    glm::vec3 normal = isTop ? glm::vec3(0, 1, 0) : glm::vec3(0, -1, 0);
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


            if (!isTop) {
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
            } else {
                // Insert first vertex
                insertVec3(m_vertexData, p1);
                insertVec3(m_vertexData, normal);
                insertVec2(m_vertexData, getUV(p1));

                // Insert third vertex
                insertVec3(m_vertexData, p3);
                insertVec3(m_vertexData, normal);
                insertVec2(m_vertexData, getUV(p3));

                // Insert second vertex
                insertVec3(m_vertexData, p2);
                insertVec3(m_vertexData, normal);
                insertVec2(m_vertexData, getUV(p2));

                // Insert first vertex
                insertVec3(m_vertexData, p4);
                insertVec3(m_vertexData, normal);
                insertVec2(m_vertexData, getUV(p4));

                // Insert third vertex
                insertVec3(m_vertexData, p6);
                insertVec3(m_vertexData, normal);
                insertVec2(m_vertexData, getUV(p6));

                // Insert second vertex
                insertVec3(m_vertexData, p5);
                insertVec3(m_vertexData, normal);
                insertVec2(m_vertexData, getUV(p5));
            }
        }
    }
}

void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    m_vertexData.clear();
    makeCap(true);    // Top cap
    makeCap(false);   // Bottom cap
    makeSide();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
void Cylinder::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}
