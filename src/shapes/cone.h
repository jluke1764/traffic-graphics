#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cone
{
public:
    Cone(int param1, int param2) {
        updateParams(param1, param2);
    }
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight, bool isTop, bool isBottom, bool isSide);
    void makeSide();
    void makeCap();
    void setVertexData();

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
