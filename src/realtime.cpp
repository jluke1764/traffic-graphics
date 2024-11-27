#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "glm/gtx/transform.hpp"
#include "settings.h"
// #include "glm/gtc/constants.hpp"
#include "utils/shaderloader.h"
#include "shapes/sphere.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/cone.h"
#include "debug.h"
#include "camera/camera.h"

void printVector(const std::string &name, const glm::vec4 &v) {
    std::cout << name << ": ";
    for (int i = 0; i<4; ++i) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
}

void printMatrix(const std::string &name, const glm::mat4 &m) {
    std::cout << name << ":" <<std::endl;
    for (int r = 0; r<4; ++r) {
        for (int c = 0; c<4; ++c) {
            std::cout << m[c][r] << " ";
        }
        std::cout << std::endl;
    }
}

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    this->doneCurrent();
}

void Realtime::updateVBOS() {
    for (int i=0; i<4; ++i) {
        // Update the vbo
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]);
        glErrorCheck();

        glBufferData(GL_ARRAY_BUFFER,shapeData[i].size() * sizeof(GLfloat),shapeData[i].data(), GL_STATIC_DRAW);
        glErrorCheck();

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glErrorCheck();
    }
}

void Realtime::tesselateShapes() {
    for (int i=0; i<shapeData.size(); ++i) {
        shapeData[i].clear();
    }
    shapeData.resize(4);
    shapeData[m_sphereIndex] = Sphere(settings.shapeParameter1, settings.shapeParameter2).generateShape();
    shapeData[m_cubeIndex] = Cube(settings.shapeParameter1).generateShape();
    shapeData[m_coneIndex] = Cone(settings.shapeParameter1, settings.shapeParameter2).generateShape();
    shapeData[m_cylinderIndex] = Cylinder(settings.shapeParameter1, settings.shapeParameter2).generateShape();

    updateVBOS();
}


void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    glErrorCheck();
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    glErrorCheck();
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    glErrorCheck();

    // Create fragment shader
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    m_vbos.resize(4);
    m_vaos.resize(4);

    glGenBuffers(4, m_vbos.data());
    glErrorCheck();

    glGenVertexArrays(4, m_vaos.data());
    glErrorCheck();

    tesselateShapes();


    for (int i=0; i<4; ++i) {
        // Bind the vbo and vao
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]);
        glErrorCheck();
        glBindVertexArray(m_vaos[i]);
        glErrorCheck();

        // Enable and define attribute 0 to store vertex positions
        glEnableVertexAttribArray(0);
        glErrorCheck();
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));
        glErrorCheck();
        glEnableVertexAttribArray(1);
        glErrorCheck();
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));
        glErrorCheck();

        // Clean-up bindings
        glBindVertexArray(0);
        glErrorCheck();
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glErrorCheck();
    }

    initialized = true;
}

void Realtime::paintGL() {

    // Students: anything requiring OpenGL calls every frame should be done here
    // Clear screen color and depth before painting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glErrorCheck();

    // Task 2: activate the shader program by calling glUseProgram with `m_shader`
    glUseProgram(m_shader);
    glErrorCheck();

    // Pass view and projection matrices to shader
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "model_view"), 1, GL_FALSE, &m_view[0][0]);
    glErrorCheck();
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "model_proj"), 1, GL_FALSE, &m_proj[0][0]);
    glErrorCheck();

    int num_shapes = m_metaData.shapes.size();

    for (int i=0; i<num_shapes; ++i){

        RenderShapeData &shape = m_metaData.shapes[i];

        glm::mat4 model = shape.ctm;

        glUniformMatrix4fv(glGetUniformLocation(m_shader, "model_matrix"), 1, GL_FALSE, &shape.ctm[0][0]);
        glErrorCheck();

        glm::mat3 it_model_matrix = glm::mat3(glm::inverse(glm::transpose(model)));

        glUniformMatrix3fv(glGetUniformLocation(m_shader, "it_model_matrix"), 1, GL_FALSE, &it_model_matrix[0][0]);
        glErrorCheck();


        glUniform4fv(glGetUniformLocation(m_shader,"O_a"),1, &shape.primitive.material.cAmbient[0]);
        glErrorCheck();
        glUniform4fv(glGetUniformLocation(m_shader,"O_d"),1, &shape.primitive.material.cDiffuse[0]);
        glErrorCheck();
        glUniform4fv(glGetUniformLocation(m_shader,"O_s"),1, &shape.primitive.material.cSpecular[0]);
        glErrorCheck();

        glUniform1f(glGetUniformLocation(m_shader,"shininess"),shape.primitive.material.shininess);
        glErrorCheck();

        int shapeIndex = m_sphereIndex;
        if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
            shapeIndex = m_sphereIndex;
        } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
            shapeIndex = m_cubeIndex;
        } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {
            shapeIndex = m_cylinderIndex;
        } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CONE) {
            shapeIndex = m_coneIndex;
        }

        // Bind the vbo and vao
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[shapeIndex]);
        glErrorCheck();

        glBindVertexArray(m_vaos[shapeIndex]);
        glErrorCheck();

        glDrawArrays(GL_TRIANGLES, 0, shapeData[shapeIndex].size() / 6);
        glErrorCheck();

        // Clean-up bindings
        glBindVertexArray(0);
        glErrorCheck();

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glErrorCheck();
    }

    // Task 3: deactivate the shader program by passing 0 into glUseProgram
    glUseProgram(0);
    glErrorCheck();
}




void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
}

void Realtime::updateLights() {
    makeCurrent();
    glErrorCheck();
    glUseProgram(m_shader);
    glErrorCheck();

    GLuint num_lights = 0;
    std::vector<Light> lights;
    for (SceneLightData &light : m_metaData.lights) {
        Light newLight;
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            newLight.type = directional;
            newLight.direction = light.dir;
        } else if (light.type == LightType::LIGHT_POINT) {
            newLight.type = point;
            newLight.position = light.pos;
            newLight.function = light.function;
        } else if (light.type == LightType::LIGHT_SPOT) {
            newLight.type = spot;
            newLight.angle = light.angle;
            newLight.penumbra = light.penumbra;
            newLight.function = light.function;
            newLight.direction = light.dir;
            newLight.position = light.pos;
        }
        newLight.color = light.color;
        lights.push_back(newLight);
        num_lights++;
    }

    for (int i=0; i<num_lights; i++) {
        glErrorCheck();
        std::string type = "lights[" + std::to_string(i) + "].type";
        glUniform1i(glGetUniformLocation(m_shader, type.c_str()), lights[i].type);
        glErrorCheck();
        std::string color = "lights[" + std::to_string(i) + "].color";
        glUniform4fv(glGetUniformLocation(m_shader, color.c_str()), 1, &lights[i].color[0]);
        glErrorCheck();
        if (lights[i].type == directional) {
            std::string direction = "lights[" + std::to_string(i) + "].direction";
            glUniform4fv(glGetUniformLocation(m_shader, direction.c_str()), 1, &lights[i].direction[0]);
            glErrorCheck();
        } else if (lights[i].type == point) {
            glErrorCheck();
            std::string position = "lights[" + std::to_string(i) + "].position";
            glUniform4fv(glGetUniformLocation(m_shader, position.c_str()), 1, &lights[i].position[0]);

            std::string function = "lights[" + std::to_string(i) + "].function";
            glUniform3fv(glGetUniformLocation(m_shader, function.c_str()), 1, &lights[i].function[0]);
            glErrorCheck();
        } else if (lights[i].type == spot) {
            glErrorCheck();
            std::cout << "angle: " << lights[i].angle << std::endl;
            std::cout << "penumbra: " << lights[i].penumbra << std::endl;
            std::cout << "function: " << lights[i].function[0] << std::endl;

            std::string position = "lights[" + std::to_string(i) + "].position";
            glUniform4fv(glGetUniformLocation(m_shader, position.c_str()), 1, &lights[i].position[0]);

            std::string direction = "lights[" + std::to_string(i) + "].direction";
            glUniform4fv(glGetUniformLocation(m_shader, direction.c_str()), 1, &lights[i].direction[0]);

            std::string angle = "lights[" + std::to_string(i) + "].angle";
            glUniform1f(glGetUniformLocation(m_shader, angle.c_str()), lights[i].angle);

            std::string penumbra = "lights[" + std::to_string(i) + "].penumbra";
            glUniform1f(glGetUniformLocation(m_shader, penumbra.c_str()), lights[i].penumbra);

            std::string function = "lights[" + std::to_string(i) + "].function";
            glUniform3fv(glGetUniformLocation(m_shader, function.c_str()), 1, &lights[i].function[0]);
            glErrorCheck();

        }
    }
    glErrorCheck();
    glUniform1i(glGetUniformLocation(m_shader, "num_lights"), num_lights);
    glErrorCheck();
    glUseProgram(0);
}

void Realtime::sceneChanged() {
    makeCurrent();
    SceneParser::parse(settings.sceneFilePath, m_metaData);
    glErrorCheck();
    Realtime::updateLights();
    glErrorCheck();

    Camera camera(m_metaData.cameraData, size().width(), size().height());

    m_view = camera.getViewMatrix();
    m_proj = camera.getProjectionMatrix();

    glUseProgram(m_shader);
    glErrorCheck();

    glUniform1f(glGetUniformLocation(m_shader,"k_a"),m_metaData.globalData.ka);
    glErrorCheck();
    glUniform1f(glGetUniformLocation(m_shader,"k_d"),m_metaData.globalData.kd);
    glErrorCheck();
    glUniform1f(glGetUniformLocation(m_shader,"k_s"),m_metaData.globalData.ks);
    glErrorCheck();

    glUniform4fv(glGetUniformLocation(m_shader,"cameraPosition"),1,&(camera.getPos()[0]));
    printVector("camera position", camera.getPos());

    glUseProgram(0);
    glErrorCheck();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (!initialized) return;

    tesselateShapes();

    Camera camera(m_metaData.cameraData, size().width(), size().height());

    m_proj = camera.getProjectionMatrix();

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    // Update map
    m_keyMap[Qt::Key(event->key())] = true;
    // // Call corresponding function
    // Camera camera(m_metaData.cameraData, size().width(), size().height());
    // glm::vec4 currentPosition = camera.getPos();
    // glm::vec4 lookVector = camera.getLook();
    // if (event->key() == Qt::Key_W) {
    //     float speed = 5.f;
    //     glm::vec4 movement = lookVector * speed * deltaTime;
    //     glm::vec4 newPosition = currentPosition + movement;
    //     camera.setPos(newPosition);
    // } else if (event->key() == Qt::Key_S) {

    // } else if (event->key() == Qt::Key_A) {

    // } else if (event->key() == Qt::Key_D) {

    // } else if (event->key() == Qt::Key_Space) {

    // } else if (event->key() == Qt::Key_Control) {

    // }
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        //implement the rodrigues matrix
        //multiply deltax by small constant to get theta
        //normalize axis of rotation, that give you ux, uy, uz
        //just use 0,1,0 for first part
        //ill end up with x rotation matrix and y rotation matrix, must apply both
        //just need to change look vector (check whether i need to deal with up too)
        //also update view matrix (global m_view variable)
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    //loop through all keys in the map and check whether they are true. Then if they are, add conditionals for what to do
    //change camera.pos and then update view matrix. maybe call an update view matrix function
    //global camera

    update(); // asks for a PaintGL() call to occur

}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
