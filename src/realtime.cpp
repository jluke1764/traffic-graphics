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
#include <cstdlib>
#include <ctime>
#include <algorithm>

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
    m_sky_shader = ShaderLoader::createShaderProgram(":/resources/shaders/skybox.vert", ":/resources/shaders/skybox.frag");

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


    float cube_size = 2;
    float v = cube_size / 2;

    std::vector<GLfloat> skybox_data = {
        // Front face
        -v, -v,  v,  v, -v,  v,  -v,  v,  v,  // Triangle 1
         v, -v,  v,  v,  v,  v,  -v,  v,  v,  // Triangle 2

        // Back face
         v, -v, -v, -v, -v, -v,   v,  v, -v,  // Triangle 1
        -v, -v, -v, -v,  v, -v,   v,  v, -v,  // Triangle 2

        // Left face
        -v, -v, -v, -v, -v,  v,  -v,  v, -v,  // Triangle 1
        -v, -v,  v, -v,  v,  v,  -v,  v, -v,  // Triangle 2

        // Right face
         v, -v,  v,  v, -v, -v,   v,  v,  v,  // Triangle 1
         v, -v, -v,  v,  v, -v,   v,  v,  v,  // Triangle 2

        // Top face
        -v,  v,  v,  v,  v,  v,  -v,  v, -v,  // Triangle 1
         v,  v,  v,  v,  v, -v,  -v,  v, -v,  // Triangle 2

        // Bottom face
        -v, -v, -v,  v, -v, -v,  -v, -v,  v,  // Triangle 1
         v, -v, -v,  v, -v,  v,  -v, -v,  v   // Triangle 2
    };

    day_sky = Skybox("resources/right.jpg","resources/left.jpg","resources/top.jpg","resources/bottom.jpg","resources/front.jpg","resources/back.jpg", m_sky_shader, GL_TEXTURE1, skybox_data.size()/3);
    night_sky = Skybox("resources/night.png","resources/night.png","resources/night.png","resources/night.png","resources/night.png","resources/night.png", m_sky_shader, GL_TEXTURE2, skybox_data.size()/3);

    glGenBuffers(1, &m_skybox_vbo);
    glErrorCheck();
    glBindBuffer(GL_ARRAY_BUFFER, m_skybox_vbo);
    glErrorCheck();
    glBufferData(GL_ARRAY_BUFFER, skybox_data.size()*sizeof(GLfloat), skybox_data.data(), GL_STATIC_DRAW);
    glErrorCheck();
    glGenVertexArrays(1, &m_skybox_vao);
    glErrorCheck();
    glBindVertexArray(m_skybox_vao);
    glErrorCheck();

    glEnableVertexAttribArray(0);
    glErrorCheck();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glErrorCheck();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glErrorCheck();
    glBindVertexArray(0);
    glErrorCheck();

    makeFBO();


    time_of_day = 12.f;
    is_night = false;

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
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(0));
        glErrorCheck();
        glEnableVertexAttribArray(1);
        glErrorCheck();
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));
        glErrorCheck();
        glEnableVertexAttribArray(2);
        glErrorCheck();
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(6 * sizeof(GLfloat)));
        glErrorCheck();

        // Clean-up bindings
        glBindVertexArray(0);
        glErrorCheck();
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glErrorCheck();
    }

    initialized = true;

    m_trafficScene = TrafficScene();
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

    // int num_shapes = m_metaData.shapes.size();

    std::vector<RenderShapeData> myShapes;
    myShapes.insert(myShapes.end(), m_metaData.shapes.begin(), m_metaData.shapes.end());
    myShapes.insert(myShapes.end(), m_trafficScene.getShapes().begin(), m_trafficScene.getShapes().end());

    int num_shapes = myShapes.size();

    for (int i=0; i<num_shapes; ++i){

        RenderShapeData &shape = myShapes[i];

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

        glUniform1i(glGetUniformLocation(m_shader,"useFog"), true); //JL
        glUniform1f(glGetUniformLocation(m_shader,"fogStart"), 10.0);
        glUniform1f(glGetUniformLocation(m_shader,"fogEnd"), 20.0);

        glUniform1i(glGetUniformLocation(m_shader, "has_texture"), shape.primitive.material.textureMap.isUsed);
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

        if (shape.primitive.material.textureMap.isUsed) {

            glUniform1f(glGetUniformLocation(m_shader, "blend"), shape.primitive.material.blend);
            glErrorCheck();

            int j = shape.primitive.material.textureMap.tex;

            // Task 9: Set the active texture slot to texture slot 1
            glActiveTexture(GL_TEXTURE3+j);
            glErrorCheck();

            glBindTexture(GL_TEXTURE_2D, m_kitten_textures[j]);
            glErrorCheck();

            // Task 6: Set min and mag filters' interpolation mode to linear
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glErrorCheck();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glErrorCheck();


            glUseProgram(m_shader);
            glErrorCheck();
            glUniform1i(glGetUniformLocation(m_shader, "tex"), 3+j);
            glErrorCheck();
        }

        // Bind the vbo and vao
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[shapeIndex]);
        glErrorCheck();

        glBindVertexArray(m_vaos[shapeIndex]);
        glErrorCheck();

        glDrawArrays(GL_TRIANGLES, 0, shapeData[shapeIndex].size() / 8);
        glErrorCheck();


        // Clean-up bindings
        glBindVertexArray(0);
        glErrorCheck();

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glErrorCheck();

        glBindTexture(GL_TEXTURE_2D, 0);
        glErrorCheck();
    }

    // for (int i=0; i<m_trafficScene.getShapes().size(); ++i){

    //     RenderShapeData &shape = m_trafficScene.getShapes()[i];

    //     glm::mat4 model = shape.ctm;

    //     glUniformMatrix4fv(glGetUniformLocation(m_shader, "model_matrix"), 1, GL_FALSE, &shape.ctm[0][0]);
    //     glErrorCheck();

    //     glm::mat3 it_model_matrix = glm::mat3(glm::inverse(glm::transpose(model)));

    //     glUniformMatrix3fv(glGetUniformLocation(m_shader, "it_model_matrix"), 1, GL_FALSE, &it_model_matrix[0][0]);
    //     glErrorCheck();


    //     glUniform4fv(glGetUniformLocation(m_shader,"O_a"),1, &shape.primitive.material.cAmbient[0]);
    //     glErrorCheck();
    //     glUniform4fv(glGetUniformLocation(m_shader,"O_d"),1, &shape.primitive.material.cDiffuse[0]);
    //     glErrorCheck();
    //     glUniform4fv(glGetUniformLocation(m_shader,"O_s"),1, &shape.primitive.material.cSpecular[0]);
    //     glErrorCheck();

    //     glUniform1f(glGetUniformLocation(m_shader,"shininess"),shape.primitive.material.shininess);
    //     glErrorCheck();

    //     int shapeIndex = m_sphereIndex;
    //     if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
    //         shapeIndex = m_sphereIndex;
    //     } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
    //         shapeIndex = m_cubeIndex;
    //     } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {
    //         shapeIndex = m_cylinderIndex;
    //     } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CONE) {
    //         shapeIndex = m_coneIndex;
    //     }

    //     // Bind the vbo and vao
    //     glBindBuffer(GL_ARRAY_BUFFER, m_vbos[shapeIndex]);
    //     glErrorCheck();

    //     glBindVertexArray(m_vaos[shapeIndex]);
    //     glErrorCheck();

    //     glDrawArrays(GL_TRIANGLES, 0, shapeData[shapeIndex].size() / 6);
    //     glErrorCheck();

    //     // Clean-up bindings
    //     glBindVertexArray(0);
    //     glErrorCheck();

    //     glBindBuffer(GL_ARRAY_BUFFER,0);
    //     glErrorCheck();
    // }

    // Task 3: deactivate the shader program by passing 0 into glUseProgram
    glUseProgram(0);
    glErrorCheck();
}

void Realtime::paintPostprocess(GLuint texture, bool invert, bool sharpen, bool grayScale, bool blur, bool sepia, bool edgeDetection){
    glUseProgram(m_texture_shader);
    // Task 32: Set your bool uniform on whether or not to filter the texture drawn
    glUniform1i(glGetUniformLocation(m_texture_shader, "invert"), invert);
    glUniform1i(glGetUniformLocation(m_texture_shader, "sharpen"), sharpen);
    glUniform1i(glGetUniformLocation(m_texture_shader, "grayScale"), grayScale);
    glUniform1i(glGetUniformLocation(m_texture_shader, "blur"), blur);
    glUniform1i(glGetUniformLocation(m_texture_shader, "sepia"), sepia);
    glUniform1i(glGetUniformLocation(m_texture_shader, "edgeDetection"), edgeDetection);
    glErrorCheck();

    glBindVertexArray(m_fullscreen_vao);
    glErrorCheck();
    // Task 10: Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glErrorCheck();
    glBindTexture(GL_TEXTURE_2D, texture);
    glErrorCheck();

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glErrorCheck();
    glBindTexture(GL_TEXTURE_2D, 0);
    glErrorCheck();
    glBindVertexArray(0);
    glErrorCheck();
    glUseProgram(0);
    glErrorCheck();
}

// void Realtime::paintTexture() {
//     glUseProgram(m_shader);
//     glErrorCheck();
//     int num_shapes = m_metaData.shapes.size();

//     for (int i=0; i<num_shapes; ++i){

//         RenderShapeData &shape = m_metaData.shapes[i];
//         glUniform1i(glGetUniformLocation(m_shader, "has_texture"), shape.primitive.material.textureMap.isUsed);
//         glErrorCheck();
//         glUniform1f(glGetUniformLocation(m_shader, "blend"), shape.primitive.material.blend);
//         glErrorCheck();
//         // Task 10: Bind "texture" to slot 1
//         if (shape.primitive.material.textureMap.isUsed) {
//             glActiveTexture(GL_TEXTURE1);
//             glErrorCheck();
//             glBindTexture(GL_TEXTURE_2D, m_kitten_texture);
//             glErrorCheck();

//             // Task 3: Generate kitten texture
//             glGenTextures(1, &m_kitten_texture);
//             glErrorCheck();

//             // Task 9: Set the active texture slot to texture slot 1
//             glActiveTexture(GL_TEXTURE1);
//             glErrorCheck();

//             // Task 4: Bind kitten texture
//             glBindTexture(GL_TEXTURE_2D, m_kitten_texture);
//             glErrorCheck();

//             // Task 5: Load image into kitten texture
//             glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
//             glErrorCheck();

//             // Task 6: Set min and mag filters' interpolation mode to linear
//             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//             glErrorCheck();
//             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//             glErrorCheck();

//             glUniform1i(glGetUniformLocation(m_shader, "tex"), 1);
//             glErrorCheck();

//             glBindVertexArray(m_vaos[i]);
//             glErrorCheck();

//             int shapeIndex = m_sphereIndex;
//             if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
//                 shapeIndex = m_sphereIndex;
//             } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
//                 shapeIndex = m_cubeIndex;
//             } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {
//                 shapeIndex = m_cylinderIndex;
//             } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CONE) {
//                 shapeIndex = m_coneIndex;
//             }

//             glBindBuffer(GL_ARRAY_BUFFER, m_vbos[shapeIndex]);
//             glErrorCheck();


//             glDrawArrays(GL_TRIANGLES, 0, shapeData[shapeIndex].size() / 8);
//             glErrorCheck();
//             glBindTexture(GL_TEXTURE_2D, 0);
//             glErrorCheck();

//             glBindVertexArray(0);
//             glErrorCheck();
//         }
//     }
//     glUseProgram(0);
//     glErrorCheck();
// }

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

    glm::mat4 cam_trans = glm::mat4(1,0,0,0,
                                  0,1,0,0,
                                  0,0,1,0,
                                  m_metaData.cameraData.pos[0], m_metaData.cameraData.pos[1], m_metaData.cameraData.pos[2], 1);

    if(is_night) {
        night_sky.Render(m_proj*m_view*cam_trans, m_sky_shader, m_skybox_vbo, m_skybox_vao, glm::vec4(1));
    } else {
        day_sky.Render(m_proj*m_view*cam_trans, m_sky_shader, m_skybox_vbo, m_skybox_vao, sun_color);
    }

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
    // paintTexture();
    paintPostprocess(m_fbo_texture, invert, sharpen, grayScale, blur, sepia, edgeDetection);
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

bool boxesOverlap(const std::array<float,4> &a, const std::array<float,4> &b) {
    bool overlapX = (a[0] < b[1]) && (a[1] > b[0]);
    bool overlapZ = (a[2] < b[3]) && (a[3] > b[2]);
    return overlapX && overlapZ;
}

void Realtime::tileCity() {
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(nullptr));
        seeded = true;
    }

    RenderData blockData;
    SceneParser::parse("scenefiles/block.json", blockData);

    int citySize = 5;
    float blockSpacing = 1.0f;
    int numBuildings = 4;
    float empireBlockChance = 0.02f;

    std::vector<RenderShapeData> baseBlockShapes;
    RenderShapeData whiteColumnData;
    bool whiteColumnFound = false;

    // Get white column primitive, ignore ctm from the file
    for (auto &s : blockData.shapes) {
        bool isWhite =
            (s.primitive.type == PrimitiveType::PRIMITIVE_CUBE) &&
            fabs(s.primitive.material.cDiffuse.r - 0.9f) < 0.001f &&
            fabs(s.primitive.material.cDiffuse.g - 0.9f) < 0.001f &&
            fabs(s.primitive.material.cDiffuse.b - 0.9f) < 0.001f;

        glm::vec3 scaleApprox(
            glm::length(glm::vec3(s.ctm[0])),
            glm::length(glm::vec3(s.ctm[1])),
            glm::length(glm::vec3(s.ctm[2]))
            );

        bool scaleMatchesWhite = (fabs(scaleApprox.x - 0.3f) < 0.05f &&
                                  fabs(scaleApprox.y - 1.0f) < 0.05f &&
                                  fabs(scaleApprox.z - 0.3f) < 0.05f);

        if (isWhite && scaleMatchesWhite && !whiteColumnFound) {
            // Copy only the primitive and material data, not the ctm
            whiteColumnData.primitive = s.primitive;
            whiteColumnFound = true;
            whiteColumnData.primitive.material.textureMap.isUsed = true;
            whiteColumnData.primitive.material.textureMap.tex = rand() % 5;
            whiteColumnData.primitive.material.blend = 1;
        } else {
            baseBlockShapes.push_back(s);
        }
    }

    for (int blockIx = -citySize; blockIx <= citySize; blockIx++) {
        for (int blockIz = -citySize; blockIz <= citySize; blockIz++) {
            float blockCenterX = blockIx * blockSpacing;
            float blockCenterZ = blockIz * blockSpacing;

            glm::mat4 blockTranslate = glm::translate(glm::vec3(blockCenterX, 0.f, blockCenterZ));

            bool isEmpireBlock = ((float)rand()/RAND_MAX) < empireBlockChance;
            bool placeWhiteColumn = false;
            if (!isEmpireBlock && whiteColumnFound) {
                placeWhiteColumn = ((float)rand()/RAND_MAX) < 0.5f;
            }

            // Place ground and roads
            for (auto &s : baseBlockShapes) {
                RenderShapeData shape = s;
                shape.ctm = blockTranslate * shape.ctm;
                m_metaData.shapes.push_back(shape);
            }

            std::vector<std::array<float,4>> placedBoxes;

            // white column
            if (!isEmpireBlock && placeWhiteColumn && whiteColumnFound) {
                float heightFactor = 0.5f + ((float)rand()/RAND_MAX)*1.0f;

                float colBaseWidth = 0.3f;
                float halfW = colBaseWidth / 2.0f;
                float maxOffset = 0.35f - halfW;
                maxOffset = std::max(0.0f, maxOffset);

                float columnHeight = 1.0f * heightFactor;

                bool placedWC = false;
                for (int attempt = 0; attempt < 10 && !placedWC; attempt++) {
                    float offsetX = ((float)rand()/RAND_MAX)*2.f*maxOffset - maxOffset;
                    float offsetZ = ((float)rand()/RAND_MAX)*2.f*maxOffset - maxOffset;

                    float worldX = blockCenterX + offsetX;
                    float worldZ = blockCenterZ + offsetZ;

                    // Bottom at y=0, so top is at y=columnHeight, center at columnHeight/2
                    float yPos = columnHeight / 2.0f;

                    glm::mat4 wcTransform = glm::translate(glm::vec3(worldX, yPos, worldZ))
                                            * glm::scale(glm::vec3(colBaseWidth, columnHeight, colBaseWidth));

                    RenderShapeData wc = whiteColumnData;
                    wc.ctm = wcTransform;
                    m_metaData.shapes.push_back(wc);

                    // Compute bounding box in world coords
                    float colHalfW = colBaseWidth / 2.0f;
                    float left   = worldX - colHalfW;
                    float right  = worldX + colHalfW;
                    float zMin   = worldZ - colHalfW;
                    float zMax   = worldZ + colHalfW;
                    std::array<float,4> candidateBox = {left, right, zMin, zMax};

                    bool overlap = false;
                    for (auto &box : placedBoxes) {
                        if (boxesOverlap(candidateBox, box)) {
                            overlap = true;
                            break;
                        }
                    }

                    if (overlap) {
                        // Remove last placed
                        m_metaData.shapes.pop_back();
                    } else {
                        placedBoxes.push_back(candidateBox);
                        placedWC = true;
                    }
                }
            }

            // Empire block
            if (isEmpireBlock) {
                float buildingHeight = 1.5f + ((float)rand()/RAND_MAX)*1.0f;
                float buildingWidth = 0.3f + ((float)rand()/RAND_MAX)*0.1f;

                float gray = 0.4f + ((float)rand()/RAND_MAX)*0.35f;
                float r = gray, g = gray, b = gray;

                float worldX = blockCenterX;
                float worldZ = blockCenterZ;

                RenderShapeData empireBuilding;
                empireBuilding.primitive.type = PrimitiveType::PRIMITIVE_CUBE;
                empireBuilding.primitive.material.cAmbient = glm::vec4(r*0.5f, g*0.5f, b*0.5f, 1.f);
                empireBuilding.primitive.material.cDiffuse = glm::vec4(r, g, b, 1.f);
                empireBuilding.primitive.material.cSpecular = glm::vec4(0.5f,0.5f,0.5f,1.f);
                empireBuilding.primitive.material.shininess = 10.f;
                empireBuilding.primitive.material.textureMap.isUsed = true;
                empireBuilding.primitive.material.textureMap.tex = rand() % 5;
                empireBuilding.primitive.material.blend = 1;

                glm::mat4 empireTransform = glm::translate(glm::vec3(worldX, buildingHeight/2.0f, worldZ))
                                            * glm::scale(glm::vec3(buildingWidth, buildingHeight, buildingWidth));

                empireBuilding.ctm = empireTransform;
                m_metaData.shapes.push_back(empireBuilding);

                float spireHeightFactor = 0.3f + ((float)rand()/RAND_MAX)*1.0f;
                float spireHeight = buildingHeight * spireHeightFactor;

                RenderShapeData spire;
                spire.primitive.type = PrimitiveType::PRIMITIVE_CONE;
                float rr = r*0.8f, rg = g*0.8f, rb = b*0.8f;
                spire.primitive.material.cAmbient = glm::vec4(rr*0.5f, rg*0.5f, rb*0.5f, 1.f);
                spire.primitive.material.cDiffuse = glm::vec4(rr, rg, rb, 1.f);
                spire.primitive.material.cSpecular = glm::vec4(0.5f,0.5f,0.5f,1.f);
                spire.primitive.material.shininess = 10.f;

                glm::mat4 spireTransform = glm::translate(glm::vec3(worldX, buildingHeight + spireHeight/2.0f, worldZ))
                                           * glm::scale(glm::vec3(buildingWidth * 0.5f, spireHeight, buildingWidth * 0.5f));

                spire.ctm = spireTransform;
                m_metaData.shapes.push_back(spire);
                continue;
            }

            // Normal buildings
            for (int i = 0; i < numBuildings; i++) {
                float buildingWidth = 0.1f + ((float)rand()/RAND_MAX)*0.1f;
                float halfW = buildingWidth/2.0f;
                float maxOffset = 0.35f - halfW;
                maxOffset = std::max(0.0f, maxOffset);

                float buildingHeight = 0.2f + ((float)rand()/RAND_MAX)*0.8f;

                float gray = 0.4f + ((float)rand()/RAND_MAX)*0.35f;
                float r = gray, g = gray, b = gray;

                bool placed = false;
                for (int attempt = 0; attempt < 10 && !placed; attempt++) {
                    float offsetX = ((float)rand()/RAND_MAX)*2.f*maxOffset - maxOffset;
                    float offsetZ = ((float)rand()/RAND_MAX)*2.f*maxOffset - maxOffset;

                    float worldX = blockCenterX + offsetX;
                    float worldZ = blockCenterZ + offsetZ;

                    float left   = worldX - halfW;
                    float right  = worldX + halfW;
                    float zMin   = worldZ - halfW;
                    float zMax   = worldZ + halfW;
                    std::array<float,4> candidateBox = {left, right, zMin, zMax};

                    bool overlap = false;
                    for (auto &box : placedBoxes) {
                        if (boxesOverlap(candidateBox, box)) {
                            overlap = true;
                            break;
                        }
                    }

                    if (!overlap) {
                        RenderShapeData building;
                        building.primitive.type = PrimitiveType::PRIMITIVE_CUBE;
                        building.primitive.material.cAmbient = glm::vec4(r*0.5f, g*0.5f, b*0.5f, 1.f);
                        building.primitive.material.cDiffuse = glm::vec4(r, g, b, 1.f);
                        building.primitive.material.cSpecular = glm::vec4(0.5f,0.5f,0.5f,1.f);
                        building.primitive.material.shininess = 10.f;
                        building.primitive.material.textureMap.isUsed = true;
                        building.primitive.material.textureMap.tex = rand() % 5;
                        building.primitive.material.blend = 1;

                        glm::mat4 buildingTransform = glm::translate(glm::vec3(worldX, buildingHeight/2.0f, worldZ))
                                                      * glm::scale(glm::vec3(buildingWidth, buildingHeight, buildingWidth));

                        building.ctm = buildingTransform;
                        m_metaData.shapes.push_back(building);

                        placedBoxes.push_back(candidateBox);

                        bool addRoof = ((rand() % 2) == 0);
                        if (addRoof) {
                            RenderShapeData roof;
                            roof.primitive.type = PrimitiveType::PRIMITIVE_CONE;

                            float roofFactor = 0.8f;
                            float rr = std::clamp(r*roofFactor, 0.f,1.f);
                            float rg = std::clamp(g*roofFactor, 0.f,1.f);
                            float rb = std::clamp(b*roofFactor, 0.f,1.f);

                            roof.primitive.material.cAmbient = glm::vec4(rr*0.5f, rg*0.5f, rb*0.5f, 1.f);
                            roof.primitive.material.cDiffuse = glm::vec4(rr, rg, rb, 1.f);
                            roof.primitive.material.cSpecular = glm::vec4(0.5f,0.5f,0.5f,1.f);
                            roof.primitive.material.shininess = 10.f;

                            float roofHeight = buildingHeight * 0.3f;
                            float roofBaseScale = buildingWidth * 1.1f;

                            glm::mat4 roofTransform = glm::translate(glm::vec3(worldX, buildingHeight + roofHeight/2.0f, worldZ))
                                                      * glm::scale(glm::vec3(roofBaseScale, roofHeight, roofBaseScale));

                            roof.ctm = roofTransform;
                            m_metaData.shapes.push_back(roof);
                        }

                        placed = true;
                    }
                }
            }
        }
    }
}

void Realtime::renderBuildings() {
    std::vector<std::string> files = {
        "textures/facade_windows.jpg",
        "textures/texture_brick.jpg",
        "textures/texture_geometric.jpg",
        "textures/texture_windows.jpg",
        "textures/grey_brick.jpg",
        "textures/grass.jpg",
        "textures/grass2.png",
        "textures/wheel.png"
    };
    for (int i = 0;i<files.size();++i) {
        // Prepare filepath
        QString texture_filepath = QString::fromStdString(files[i]);

        // Task 1: Obtain image from filepath
        m_image = QImage(texture_filepath);
        if (m_image.isNull()) std::cout << "Bad filepath" <<std::endl;

        // Task 2: Format image to fit OpenGL
        m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();


        // Task 3: Generate kitten texture
        glGenTextures(1, &m_kitten_textures[i]);
        glErrorCheck();

        // Task 9: Set the active texture slot to texture slot 1
        glActiveTexture(GL_TEXTURE3+i);
        glErrorCheck();

        // Task 4: Bind kitten texture
        glBindTexture(GL_TEXTURE_2D, m_kitten_textures[i]);
        glErrorCheck();

        // Task 5: Load image into kitten texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
        glErrorCheck();

        // Task 7: Unbind kitten texture

        glBindTexture(GL_TEXTURE_2D, 0);
        glErrorCheck();

        glUseProgram(0);
        glErrorCheck();
    }
}


void Realtime::sceneChanged() {
    makeCurrent();
    SceneParser::parse(settings.sceneFilePath, m_metaData);
    glErrorCheck();

    // m_trafficScene.getShapes(m_metaData.shapes);
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

    setTimeOfDay();

    renderBuildings();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::setTimeOfDay() {
    float sunrise = 6.f;
    float sunset = 20.f;

    if(time_of_day > sunset || time_of_day < sunrise) { //night
        sun_color = glm::vec4(.0, .0, .0, 1);
        is_night = true;
    } else {
        is_night = false;
        float day_pcnt = (time_of_day - sunrise)/(sunset - sunrise);
        float angle = glm::radians(180.f * day_pcnt);

        m_metaData.lights[0].dir = glm::vec4(glm::cos(angle), -glm::sin(angle), 0, 0);

        float intensity = 1.f - abs(.5f-day_pcnt);
        if(day_pcnt > .85f) { // sunset
            sun_color = glm::vec4(1, .47, .44, 1) * intensity;
        } else {
            sun_color = glm::vec4(1, 1, 1, 1) * intensity;
        }
    }
    m_metaData.lights[0].color = sun_color;
    updateLights();
}

void Realtime::settingsChanged() {
    if (!initialized) return;
    if (param1 != settings.shapeParameter1 || param2 != settings.shapeParameter2) {
        tesselateShapes();
    }
    if (_near != settings.nearPlane || _far != settings.farPlane) {
        Camera camera(m_metaData.cameraData, size().width(), size().height());
        m_proj = camera.getProjectionMatrix();
    }
    param1 = settings.shapeParameter1;
    param2 = settings.shapeParameter2;
    _near = settings.nearPlane;
    _far = settings.farPlane;

    if(time_of_day != settings.sun) {
        time_of_day = settings.sun;
        setTimeOfDay();
    }

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
    // // Call corresponding function
    Camera camera(m_metaData.cameraData, size().width(), size().height());
    glm::vec4 currentPosition = camera.getPos();
    glm::vec4 lookVector = camera.getLook();
    if (event->key() == Qt::Key_W) {
        // float speed = 5.f;
        // glm::vec4 movement = lookVector * speed * deltaTime;
        // glm::vec4 newPosition = currentPosition + movement;
        // camera.setPos(newPosition);
    } else if (event->key() == Qt::Key_S) {

    } else if (event->key() == Qt::Key_A) {

    } else if (event->key() == Qt::Key_D) {

    } else if (event->key() == Qt::Key_Space) {
        m_tickCount++;
        m_trafficScene.update(m_tickCount);
    } else if (event->key() == Qt::Key_Control) {

    }

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
    if (m_keyMap[Qt::Key_Shift]) {
        m_metaData.cameraData.pos += glm::vec4(0,1,0,0)*(5.f*deltaTime);
        updateViewMatrix();
    }
    if (m_keyMap[Qt::Key_Control]) {
        m_metaData.cameraData.pos += glm::vec4(0,-1,0,0)*(5.f*deltaTime);
        updateViewMatrix();
    }

    //update car movement
    // m_trafficScene.update(m_tickCount);
    // m_tickCount++;

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
