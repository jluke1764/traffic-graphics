#include "sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    makeSphere();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    float radius = 0.5f;
    float phi_distance = M_PI / m_param1;

    float sin_curr_theta = sin(currentTheta);
    float cos_curr_theta = cos(currentTheta);

    float sin_next_theta = sin(nextTheta);
    float cos_next_theta = cos(nextTheta);

    float currentPhi = 0;

    float cos_curr_phi = cos(currentPhi);
    float sin_curr_phi = sin(currentPhi);
    for (int i = 0; i < m_param1; ++i) {

        float nextPhi = (i+1)*phi_distance;

        float cos_next_phi = cos(nextPhi);
        float sin_next_phi = sin(nextPhi);


        glm::vec3 topLeft = radius * glm::vec3(sin_curr_phi * sin_curr_theta, cos_curr_phi, sin_curr_phi * cos_curr_theta);

        glm::vec3 topRight = radius * glm::vec3(sin_curr_phi * sin_next_theta, cos_curr_phi, sin_curr_phi * cos_next_theta);

        glm::vec3 bottomLeft = radius * glm::vec3(sin_next_phi * sin_curr_theta, cos_next_phi, sin_next_phi * cos_curr_theta);

        glm::vec3 bottomRight = radius * glm::vec3(sin_next_phi * sin_next_theta, cos_next_phi, sin_next_phi * cos_next_theta);

        makeTile(topLeft, topRight, bottomLeft, bottomRight);
        cos_curr_phi = cos_next_phi;
        sin_curr_phi = sin_next_phi;
    }
}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    float theta_distance = 2*M_PI / m_param2;

    for (int i = 0; i < m_param2; ++i) {
        float currentTheta = i * theta_distance + glm::radians(90.f);
        float nextTheta = (i + 1) * theta_distance + glm::radians(90.f);

        makeWedge(currentTheta, nextTheta);
    }
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
