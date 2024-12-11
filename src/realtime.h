#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "camera/camera.h"
#include "settings.h"
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

#include "skybox.h"


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
    void initializeGeometry();
    void makeFBO();
    void paintGeometry();
    void paintPostprocess(GLuint texture, bool perPixel, bool kernel, bool grayScale, bool blur, bool sepia, bool edgeDetection);
    void paintTexture();
    void paintGL() override;          // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void rotateView(glm::vec3 axis, float theta);

    void mouseMoveEvent(QMouseEvent *event) override;
    void updateViewMatrix();
    void timerEvent(QTimerEvent *event) override;

    void updateVBOS();
    void tesselateShapes();
    void updateLights();
    void tileCity();
    void setTimeOfDay();

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
    GLuint m_texture_shader;
    GLuint m_shader;     // Stores id of shader program
    GLuint m_sky_shader;
    std::vector<GLuint> m_vaos; // Stores id of vao
    std::vector<GLuint> m_vbos;

    GLuint m_fbo_texture;
    std::vector<GLuint> m_kitten_textures = std::vector<GLuint>(100);
    GLuint m_fbo_renderbuffer;
    GLuint m_fbo;
    GLuint m_defaultFBO;

    QImage m_image;

    int m_fbo_width;
    int m_fbo_height;

    int m_screen_width;
    int m_screen_height;

    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

    GLuint m_skybox_vbo;
    GLuint m_skybox_vao;

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

    float param1 = settings.shapeParameter1;
    float param2 = settings.shapeParameter2;
    float _near = settings.nearPlane;
    float _far = settings.farPlane;
    float time_of_day = settings.sun;

    Skybox day_sky;
    Skybox night_sky;
    glm::vec4 sun_color;
    bool is_night;
};
