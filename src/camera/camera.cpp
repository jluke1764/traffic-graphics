// #include <stdexcept>
#include "camera.h"
#include "settings.h"
// #include "parser/scenefilereader.h"
// #include <iostream>
#include <glm/gtc/matrix_transform.hpp>


glm::mat4 Camera::getViewMatrix() const {
    // Optional TODO: implement the getter or make your own design
    // std::cout << "look" << m_cameraData.look[0] << m_cameraData.look[1] << m_cameraData.look[2] << std::endl;
    glm::vec3 w = -m_cameraData.look;

    w = glm::normalize(w);
    glm::vec3 up = m_cameraData.up;
    glm::vec3 v = up - glm::dot(up,w)*w;
    v = glm::normalize(v);
    glm::vec3 u = glm::cross(v,w);
    glm::mat4 translate;
    translate[0] = glm::vec4(1,0,0,0);
    translate[1] = glm::vec4(0,1,0,0);
    translate[2] = glm::vec4(0,0,1,0);
    translate[3] = glm::vec4(-m_cameraData.pos[0],-m_cameraData.pos[1],-m_cameraData.pos[2],1);
    glm::mat4 rotate;
    rotate[0] = glm::vec4(u,0);
    rotate[1] = glm::vec4(v,0);
    rotate[2] = glm::vec4(w,0);
    rotate[3] = glm::vec4(0,0,0,1);
    rotate = glm::transpose(rotate);

    return rotate*translate;
}
glm::mat4 Camera::getProjectionMatrix() const {
    float thetaH = m_cameraData.heightAngle;
    // float thetaW = 2*tan((m_width/m_height)*(atan(thetaH/2)));
    float far = settings.farPlane;
    float near = settings.nearPlane;

    glm::mat4 scale;
    scale[0] = glm::vec4(1/(far*(m_width/m_height)*tan(thetaH/2)),0,0,0);
    scale[1] = glm::vec4(0,1/(far*tan(thetaH/2)),0,0);
    scale[2] = glm::vec4(0,0,1/far,0);
    scale[3] = glm::vec4(0,0,0,1);

    float c = -near/far;
    glm::mat4 unhinge;
    unhinge[0] = glm::vec4(1,0,0,0);
    unhinge[1] = glm::vec4(0,1,0,0);
    unhinge[2] = glm::vec4(0,0,1/(1+c),-1);
    unhinge[3] = glm::vec4(0,0,-c/(1+c),0);

    glm::mat4 remap;
    remap[0] = glm::vec4(1,0,0,0);
    remap[1] = glm::vec4(0,1,0,0);
    remap[2] = glm::vec4(0,0,-2,0);
    remap[3] = glm::vec4(0,0,-1,1);
    return remap*unhinge*scale;
}

float Camera::getAspectRatio() const {
    // Optional TODO: implement the getter or make your own design
    return m_width/m_height;
}

float Camera::getHeightAngle() const {
    // Optional TODO: implement the getter or make your own design
    return m_cameraData.heightAngle;
}

float Camera::getFocalLength() const {
    // Optional TODO: implement the getter or make your own design
    return m_cameraData.focalLength;
}

float Camera::getAperture() const {
    // Optional TODO: implement the getter or make your own design
    return m_cameraData.aperture;
}

glm::vec4 Camera::getPos() const {
    return m_cameraData.pos;
}

void Camera::setPos(glm::vec4 position) {
    m_cameraData.pos = position;
}

glm::vec4 Camera::getLook() const {
    return m_cameraData.look;
}

float Camera::getCameraWidth() const {
    return m_width;
}
float Camera::getCameraHeight() const {
    return m_height;
}
