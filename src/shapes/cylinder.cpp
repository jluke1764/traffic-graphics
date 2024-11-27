#include "cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

// makeTile(tL, Tl... normal for each position)
// makeCap(isTop) // switch insert order based on whether it's the top or bottom
// length/param
// edge case for top


// void Cylinder::makeSideTile(glm::vec3 topLeft,
//                       glm::vec3 topRight,
//                       glm::vec3 bottomLeft,
//                       glm::vec3 bottomRight) {
//     insertVec3(m_vertexData, topLeft);
//     insertVec3(m_vertexData, glm::normalize(glm::vec3(topLeft.x, 0, topLeft.z)));

//     insertVec3(m_vertexData, bottomLeft);
//     insertVec3(m_vertexData, glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z)));

//     insertVec3(m_vertexData, bottomRight);
//     insertVec3(m_vertexData, glm::normalize(glm::vec3(bottomRight.x, 0, bottomRight.z)));

//     insertVec3(m_vertexData, topLeft);
//     insertVec3(m_vertexData, glm::normalize(glm::vec3(topLeft.x, 0, topLeft.z)));

//     insertVec3(m_vertexData, bottomRight);
//     insertVec3(m_vertexData, glm::normalize(glm::vec3(bottomRight.x, 0, bottomRight.z)));

//     insertVec3(m_vertexData, topRight);
//     insertVec3(m_vertexData, glm::normalize(glm::vec3(topRight.x, 0, topRight.z)));
// }

// void Cylinder::makeFaceTile(glm::vec3 topLeft,
//                             glm::vec3 topRight,
//                             glm::vec3 bottomLeft,
//                             glm::vec3 bottomRight,
//                             bool isTop) {
//     glm::vec3 normal;
//     if (isTop) {
//         normal = glm::vec3(0, 1, 0);
//     } else if (!isTop) {
//         normal = glm::vec3(0, -1, 0);
//     }
//     insertVec3(m_vertexData, topLeft);
//     insertVec3(m_vertexData, normal);

//     insertVec3(m_vertexData, bottomLeft);
//     insertVec3(m_vertexData, normal);

//     insertVec3(m_vertexData, bottomRight);
//     insertVec3(m_vertexData, normal);

//     insertVec3(m_vertexData, topLeft);
//     insertVec3(m_vertexData, normal);

//     insertVec3(m_vertexData, bottomRight);
//     insertVec3(m_vertexData, normal);

//     insertVec3(m_vertexData, topRight);
//     insertVec3(m_vertexData, normal);
// }

// void Cylinder::makeWedge(float currentTheta, float nextTheta, bool isTop) {
//     // Task 6: create a single wedge of the sphere using the
//     //         makeTile() function you implemented in Task 5
//     // Note: think about how param 1 comes into play here!
//     float radius = 0.5f;
//     float phi_distance = 3.14159 / m_param1;

//     for (int i = 0; i < m_param1; ++i) {
//         float currentPhi = i*phi_distance;
//         float nextPhi = (i+1)*phi_distance;

//         glm::vec3 topLeft = radius * glm::vec3(sin(currentPhi) * sin(currentTheta), cos(currentPhi), sin(currentPhi) * cos(currentTheta));

//         glm::vec3 topRight = radius * glm::vec3(sin(currentPhi) * sin(nextTheta), cos(currentPhi), sin(currentPhi) * cos(nextTheta));

//         glm::vec3 bottomLeft = radius * glm::vec3(sin(nextPhi) * sin(currentTheta), cos(nextPhi), sin(nextPhi) * cos(currentTheta));

//         glm::vec3 bottomRight = radius * glm::vec3(sin(nextPhi) * sin(nextTheta), cos(nextPhi), sin(nextPhi) * cos(nextTheta));

//         if
//         makeTile(topLeft, topRight, bottomLeft, bottomRight);
//     }
// }

// void Cylinder::makeCylinder() {
//     // Task 7: create a full sphere using the makeWedge() function you
//     //         implemented in Task 6
//     // Note: think about how param 2 comes into play here!
//     float theta_distance = 2*3.14159 / m_param2;

//     for (int i = 0; i < m_param2; ++i) {
//         float currentTheta = i * theta_distance;
//         float nextTheta = (i + 1) * theta_distance;

//         makeWedge(currentTheta, nextTheta);
//     }
// }

// void Cylinder::setVertexData() {
//     // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:

//     float thetaStep = glm::radians(360.f / m_param2);
//     float currentTheta = 0 * thetaStep;
//     float nextTheta = 1 * thetaStep;
//     makeWedge(currentTheta, nextTheta);

//     // Uncomment these lines to make sphere for Task 7:

//     makeCylinder();
// }





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

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalBR);

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normalBL);


        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normalTL);


        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normalTR);

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalBR);
    }
    if (isBottom) {
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normal);


        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);


        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normal);

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);

    } else if (isTop) {
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normal);

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);

        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);

        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normal);
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

                // Insert second vertex
                insertVec3(m_vertexData, p2);
                insertVec3(m_vertexData, normal);

                // Insert third vertex
                insertVec3(m_vertexData, p3);
                insertVec3(m_vertexData, normal);

                // Insert first vertex
                insertVec3(m_vertexData, p4);
                insertVec3(m_vertexData, normal);

                // Insert second vertex
                insertVec3(m_vertexData, p5);
                insertVec3(m_vertexData, normal);

                // Insert third vertex
                insertVec3(m_vertexData, p6);
                insertVec3(m_vertexData, normal);
            } else {
                // Insert first vertex
                insertVec3(m_vertexData, p1);
                insertVec3(m_vertexData, normal);

                // Insert third vertex
                insertVec3(m_vertexData, p3);
                insertVec3(m_vertexData, normal);

                // Insert second vertex
                insertVec3(m_vertexData, p2);
                insertVec3(m_vertexData, normal);

                // Insert first vertex
                insertVec3(m_vertexData, p4);
                insertVec3(m_vertexData, normal);

                // Insert third vertex
                insertVec3(m_vertexData, p6);
                insertVec3(m_vertexData, normal);

                // Insert second vertex
                insertVec3(m_vertexData, p5);
                insertVec3(m_vertexData, normal);
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
