#include "cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    int i, int j) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 normal1 = glm::normalize(glm::cross(bottomLeft-topLeft, bottomRight-topLeft));
    glm::vec3 normal2 = glm::normalize(glm::cross(bottomRight-topLeft, topRight-topLeft));

    float u0 = float(i)/m_param1;
    float u1 = float(i+1)/m_param1;
    float v0 = 1-float(j+1)/m_param1;
    float v1 = 1-float(j)/m_param1;

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal1);
    insertVec2(m_vertexData, glm::vec2(u0,v1));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal1);
    insertVec2(m_vertexData, glm::vec2(u0,v0));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal1);
    insertVec2(m_vertexData, glm::vec2(u1,v0));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal2);
    insertVec2(m_vertexData, glm::vec2(u0,v1));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal2);
    insertVec2(m_vertexData, glm::vec2(u1,v0));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal2);
    insertVec2(m_vertexData, glm::vec2(u1,v1));
}



void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube
    // num_triangles = 2*pow(m_param1, 2);
    glm::vec3 x_distance = (topRight - topLeft) / static_cast<float>(m_param1);
    glm::vec3 y_distance = (bottomLeft - topLeft) / static_cast<float>(m_param1);

    // Loop over the grid of tiles
    for (float i = 0; i < m_param1; ++i) {
        for (float j = 0; j < m_param1; ++j) {
            glm::vec3 TL = topLeft + x_distance*i + y_distance*j;
            glm::vec3 TR = TL + x_distance;
            glm::vec3 BL = TL + y_distance;
            glm::vec3 BR = BL + x_distance;

            makeTile(TL, TR, BL, BR, i, j);
        }
    }
}

void Cube::setVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:

    // makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
    //          glm::vec3( 0.5f,  0.5f, 0.5f),
    //          glm::vec3(-0.5f, -0.5f, 0.5f),
    //          glm::vec3( 0.5f, -0.5f, 0.5f));

    // Uncomment these lines for Task 3:
    //front face
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));


    // Task 4: Use the makeFace() function to make all 6 sides of the cube
    //left face
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f,  0.5f));

    //back face
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));

    //right face
    makeFace(glm::vec3( 0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));

    //top face
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f,  0.5f));

    //bottom face
    makeFace(glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
void Cube::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}
