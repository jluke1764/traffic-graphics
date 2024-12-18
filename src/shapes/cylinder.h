#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cylinder
{
public:
    Cylinder(int param1, int param2) {
        updateParams(param1, param2);
    }
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void insertVec2(std::vector<float> &data, glm::vec2 v);
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight, bool isTop, bool isBottom, bool isSide);
    void makeSide();
    void makeCap(bool isTop);
    glm::vec2 getUV(glm::vec3 point);

    std::vector<float> m_vertexData;
    float m_radius = 0.5;
    int m_param1;
    int m_param2;
};
