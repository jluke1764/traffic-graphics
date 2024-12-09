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
void printVector3(const std::string &name, const glm::vec3 &v) {
    std::cout << name << ": ";
    for (int i = 0; i<3; ++i) {
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
    glDeleteProgram(m_texture_shader);
    glErrorCheck();
    glDeleteProgram(m_shader);
    glErrorCheck();
    for (int i=0; i<4; ++i) {
        glDeleteVertexArrays(1, &m_vaos[i]);
        glErrorCheck();
        glDeleteBuffers(1, &m_vbos[i]);
        glErrorCheck();
    }
    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glErrorCheck();
    glDeleteBuffers(1, &m_fullscreen_vbo);
    glErrorCheck();

    // Task 35: Delete OpenGL memory here
    glDeleteTextures(1,&m_fbo_texture);
    glErrorCheck();
    glDeleteRenderbuffers(1,&m_fbo_renderbuffer);
    glErrorCheck();
    glDeleteFramebuffers(1,&m_fbo);
    glErrorCheck();

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

    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

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
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    initializeGeometry();

    // Task 10: Set the texture.frag uniform for our texture
    glUseProgram(m_texture_shader);
    glErrorCheck();
    glUniform1i(glGetUniformLocation(m_texture_shader, "tex"), 0);
    glErrorCheck();
    glUseProgram(0);
    glErrorCheck();

    // Task 11: Fix this "fullscreen" quad's vertex data
    // Task 12: Play around with different values!
    // Task 13: Add UV coordinates
    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
            -1.f,  1.f, 0.0f, 0.0f,1.0f,
            -1.f, -1.f, 0.0f, 0.0f,0.0f,
            1.f, 1.f, 0.0f, 1.0f,1.0f,
            1.f,  1.f, 0.0f, 1.0f,1.0f,
            -1.f,  -1.f, 0.0f, 0.0f,0.0f,
            1.f, -1.f, 0.0f, 1.0f,0.0f
        };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glErrorCheck();
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glErrorCheck();
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glErrorCheck();
    glGenVertexArrays(1, &m_fullscreen_vao);
    glErrorCheck();
    glBindVertexArray(m_fullscreen_vao);
    glErrorCheck();

    // Task 14: modify the code below to add a second attribute to the vertex attribute array
    glEnableVertexAttribArray(0);
    glErrorCheck();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glErrorCheck();

    glEnableVertexAttribArray(1);
    glErrorCheck();
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(3*sizeof(GLfloat)));
    glErrorCheck();

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glErrorCheck();
    glBindVertexArray(0);
    glErrorCheck();

    makeFBO();
    initialized = true;
}

void Realtime::initializeGeometry() {
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
    // makeFBO();
    // initialized = true;
}

void Realtime::makeFBO(){
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_texture);
    glErrorCheck();

    glActiveTexture(GL_TEXTURE0);
    glErrorCheck();

    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glErrorCheck();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glErrorCheck();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glErrorCheck();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glErrorCheck();

    glBindTexture(GL_TEXTURE_2D, 0);
    glErrorCheck();
    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glErrorCheck();
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glErrorCheck();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glErrorCheck();
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glErrorCheck();

    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glErrorCheck();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glErrorCheck();
    // Task 21: Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glErrorCheck();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);
    glErrorCheck();
    // Task 22: Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glErrorCheck();
    // auto err = glGetError();
    // std::cout << gluErrorString(err) << " err " << std::endl;
}

void Realtime::paintGeometry() {

    // Students: anything requiring OpenGL calls every frame should be done here
    // Clear screen color and depth before painting
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glErrorCheck();

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

void Realtime::paintTexture(GLuint texture, bool invert, bool sharpen, bool grayScale, bool blur, bool sepia, bool edgeDetection){
    glUseProgram(m_texture_shader);
    // Task 32: Set your bool uniform on whether or not to filter the texture drawn
    glUniform1i(glGetUniformLocation(m_texture_shader, "invert"), invert);
    glUniform1i(glGetUniformLocation(m_texture_shader, "sharpen"), sharpen);
    glUniform1i(glGetUniformLocation(m_texture_shader, "grayScale"), grayScale);
    glUniform1i(glGetUniformLocation(m_texture_shader, "blur"), blur);
    glUniform1i(glGetUniformLocation(m_texture_shader, "sepia"), sepia);
    glUniform1i(glGetUniformLocation(m_texture_shader, "edgeDetection"), edgeDetection);

    glBindVertexArray(m_fullscreen_vao);
    // Task 10: Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::paintGL() {
    // Task 24: Bind our FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glErrorCheck();

    // Task 28: Call glViewport
    glViewport(0,0,m_fbo_width, m_fbo_height);
    glErrorCheck();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glErrorCheck();
    paintGeometry();

    // Task 25: Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glErrorCheck();

    // glViewport(0,0,m_screen_width, m_screen_height);
    // glErrorCheck();

    // Task 26: Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glErrorCheck();

    // Task 27: Call paintTexture to draw our FBO color attachment texture | Task 31: Set bool parameter to true
    bool invert;
    bool sharpen;
    bool grayScale;
    bool blur;
    bool sepia;
    bool edgeDetection;

    if (settings.perPixelFilter) {
        invert = true;
    } else {
        invert = false;
    }
    if (settings.kernelBasedFilter) {
        sharpen = true;
    } else {
        sharpen = false;
    }
    if (settings.extraCredit1) {
        grayScale = true;
    } else {
        grayScale = false;
    }
    if (settings.extraCredit2) {
        blur = true;
    } else {
        blur = false;
    }
    if (settings.extraCredit3) {
        sepia = true;
    } else {
        sepia = false;
    }
    if (settings.extraCredit4) {
        edgeDetection = true;
    } else {
        edgeDetection = false;
    }
    paintTexture(m_fbo_texture, invert, sharpen, grayScale, blur, sepia, edgeDetection);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    glErrorCheck();

    // Students: anything requiring OpenGL calls when the program starts should be done here
    // Task 34: Delete Texture, Renderbuffer, and Framebuffer memory
    glDeleteTextures(1,&m_fbo_texture);
    glErrorCheck();

    glDeleteRenderbuffers(1,&m_fbo_renderbuffer);
    glErrorCheck();

    glDeleteFramebuffers(1,&m_fbo);
    glErrorCheck();

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    // Task 34: Regenerate your FBOs
    makeFBO();

    m_proj = glm::perspective(glm::radians(45.0), 1.0 * w / h, 0.01, 100.0);
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

void Realtime::tileCity() {
    RenderData block;

    SceneParser::parse("scenefiles/block.json", block);

    for(int x = -5; x <= 5; x++) {
        for(int z = -5; z <= 5; z++) {
            glm::mat4 translation = glm::transpose(glm::mat4(1,0,0,x,
                                                       0,1,0,0,
                                                       0,0,1,z,
                                                       0,0,0,1));

            for(RenderShapeData s : block.shapes) {
                s.ctm = translation * s.ctm;
                m_metaData.shapes.push_back(s);
            }
        }
    }
}

void Realtime::sceneChanged() {
    makeCurrent();
    SceneParser::parse(settings.sceneFilePath, m_metaData);
    glErrorCheck();

    tileCity();

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

    glUseProgram(0);
    glErrorCheck();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (!initialized) return;
    if (param1 != settings.shapeParameter1 || param2 != settings.shapeParameter2) {
        tesselateShapes();
    }
    if (near != settings.nearPlane || far != settings.farPlane) {
        Camera camera(m_metaData.cameraData, size().width(), size().height());
        m_proj = camera.getProjectionMatrix();
    }
    param1 = settings.shapeParameter1;
    param2 = settings.shapeParameter2;
    near = settings.nearPlane;
    far = settings.farPlane;

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
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

void Realtime::rotateView(glm::vec3 axis, float theta) {
    float ux = axis[0];
    float uy = axis[1];
    float uz = axis[2];

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    glm::mat3 rotation(cosTheta + pow(ux,2)*(1-cosTheta), ux*uy*(1-cosTheta)-uz*sinTheta, ux*uz*(1-cosTheta)+uy*sinTheta,
                       ux*uy*(1-cosTheta)+uz*sinTheta, cosTheta+pow(uy,2)*(1-cosTheta), uy*uz*(1-cosTheta)-ux*sinTheta,
                       ux*uz*(1-cosTheta)-uy*sinTheta, uy*uz*(1-cosTheta)+ux*sinTheta, cosTheta+pow(uz,2)*(1-cosTheta));

    // printMatrix("rotation matrix", rotation);

    m_metaData.cameraData.look = glm::vec4(glm::normalize(glm::vec3(m_metaData.cameraData.look)*rotation),0);
    m_metaData.cameraData.up = glm::vec4(glm::normalize(glm::vec3(m_metaData.cameraData.up)*rotation),0);
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        glm::vec3 x_axis(0,1,0);
        glm::vec3 y_axis(glm::normalize(glm::cross(glm::vec3(m_metaData.cameraData.look), glm::vec3(m_metaData.cameraData.up))));
        // printVector("y_axis", glm::vec4(y_axis, 0));

        // get radians moved per pixel
        float radiansPerPixel = m_metaData.cameraData.heightAngle/size().height();

        rotateView(x_axis, deltaX*radiansPerPixel);
        rotateView(y_axis, deltaY*radiansPerPixel);

        // rotateView(x_axis, deltaX*0.1);
        // rotateView(y_axis, deltaY*0.1);

        Camera camera(m_metaData.cameraData, size().width(), size().height());
        m_view = camera.getViewMatrix();

        update(); // asks for a PaintGL() call to occur

        // Use deltaX and deltaY here to rotate
        //implement the rodrigues matrix
        //multiply deltax by small constant to get theta
        //normalize axis of rotation, that give you ux, uy, uz
        //just use 0,1,0 for first part
        //ill end up with x rotation matrix and y rotation matrix, must apply both
        //just need to change look vector (check whether i need to deal with up too)
        //also update view matrix (global m_view variable)
    }
}

void Realtime::updateViewMatrix() {
    Camera camera(m_metaData.cameraData, size().width(), size().height());
    m_view = camera.getViewMatrix();

    glUniform4fv(glGetUniformLocation(m_shader,"cameraPosition"),1,&(camera.getPos()[0]));
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    //loop through all keys in the map and check whether they are true. Then if they are, add conditionals for what to do
    //change camera.pos and then update view matrix. maybe call an update view matrix function
    //global camera

    if (m_keyMap[Qt::Key_W]) {
        m_metaData.cameraData.pos += glm::normalize(m_metaData.cameraData.look)*(5.f*deltaTime);
        updateViewMatrix();
    }
    if (m_keyMap[Qt::Key_S]) {
        m_metaData.cameraData.pos += -glm::normalize(m_metaData.cameraData.look)*(5.f*deltaTime);
        updateViewMatrix();
    }
    if (m_keyMap[Qt::Key_A]) {
        m_metaData.cameraData.pos -= glm::vec4(glm::cross(glm::normalize(glm::vec3(m_metaData.cameraData.look)), glm::normalize(glm::vec3(m_metaData.cameraData.up))),0)*5.f*deltaTime;
        updateViewMatrix();
    }
    if (m_keyMap[Qt::Key_D]) {
        m_metaData.cameraData.pos += glm::vec4(glm::cross(glm::normalize(glm::vec3(m_metaData.cameraData.look)), glm::normalize(glm::vec3(m_metaData.cameraData.up))),0)*5.f*deltaTime;
        updateViewMatrix();
    }
    if (m_keyMap[Qt::Key_Space]) {
        m_metaData.cameraData.pos += glm::vec4(0,1,0,0)*(5.f*deltaTime);
        updateViewMatrix();
    }
    if (m_keyMap[Qt::Key_Control]) {
        m_metaData.cameraData.pos += glm::vec4(0,-1,0,0)*(5.f*deltaTime);
        updateViewMatrix();
    }

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
