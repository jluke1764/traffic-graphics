#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "parser/sceneparser.h"
#include "shapes/cube.h"
#include "shapes/cone.h"
#include "shapes/sphere.h"
#include "shapes/cylinder.h"


class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    void updateVBOS();
    void tesselateShapes();
    void updateLights();

    struct Light {
        int type;
        glm::vec4 direction;
        glm::vec4 color;
        glm::vec4 position;
        glm::vec3 function;
        float angle;
        float penumbra;
    };
    const int directional = 0;
    const int point = 1;
    const int spot = 2;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;

    RenderData m_metaData;
    GLuint m_shader;     // Stores id of shader program
    std::vector<GLuint> m_vaos; // Stores id of vao
    std::vector<GLuint> m_vbos;

    std::vector<std::vector<float>> shapeData;

    const int m_sphereIndex = 0;
    const int m_cubeIndex = 1;
    const int m_coneIndex = 2;
    const int m_cylinderIndex = 3;

    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);

    glm::vec4 m_lightPos; // The world-space position of the point light

    QPoint m_prevMousePos;
    float  m_angleX;
    float  m_angleY;
    float  m_zoom;

    bool initialized = false;
};
