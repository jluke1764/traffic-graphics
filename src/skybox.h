#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

class Skybox {
public:
    Skybox(const std::string& positive_x_path,
           const std::string& negative_x_path,
           const std::string& positive_y_path,
           const std::string& negative_y_path,
           const std::string& positive_z_path,
           const std::string& negative_z_path,
           GLuint shader,
           GLuint texture_slot,
           int verts);

    Skybox();

    ~Skybox();

    void Render(glm::mat4 mvp, GLuint shader_program, GLuint vbo, GLuint vao);
private:
    GLuint m_texture;
    GLuint m_slot;
    std::string m_fileNames[6];
    int skybox_verts;
};

#endif // SKYBOX_H
