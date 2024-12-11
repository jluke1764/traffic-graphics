#pragma once

#include <vector>
#include <glm/glm.hpp>

class Sphere
{
public:
    Sphere(int param1, int param2) {
        updateParams(param1, param2);
    }
    void updateParams(int param1, int param2);
    glm::vec2 getUV(float x, float y, float z);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    Sphere() = delete;
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void insertVec2(std::vector<float> &data, glm::vec2 v);
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeWedge(float currTheta, float nextTheta);
    void makeSphere();

    std::vector<float> m_vertexData;
    float m_radius = 0.5;
    int m_param1;
    int m_param2;
};
