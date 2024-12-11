#include "skybox.h"
#include <QString>
#include <QImage>
#include <iostream>
#include "debug.h"

Skybox::Skybox() {

}

Skybox::~Skybox() {

}

Skybox::Skybox(const std::string& positive_x_path,
       const std::string& negative_x_path,
       const std::string& positive_y_path,
       const std::string& negative_y_path,
       const std::string& positive_z_path,
       const std::string& negative_z_path,
               GLuint shader,
               GLuint texture_slot,
               int verts)
{
    skybox_verts = verts;
    m_slot = texture_slot;

    m_fileNames[0] = positive_x_path;
    m_fileNames[1] = negative_x_path;
    m_fileNames[2] = positive_y_path;
    m_fileNames[3] = negative_y_path;
    m_fileNames[4] = positive_z_path;
    m_fileNames[5] = negative_z_path;

    GLenum faces[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};

    glActiveTexture(m_slot);
    glErrorCheck();

    glGenTextures(1, &m_texture);
    glErrorCheck();

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
    glErrorCheck();

    for(int i = 0; i < 6; i++) {
        QString img_filepath = QString::fromStdString(m_fileNames[i]);

        QImage m_image = QImage(img_filepath);

        m_image = m_image.convertToFormat(QImage::Format_RGB888);

        glTexImage2D(faces[i], 0, GL_RGB, m_image.width(), m_image.height(), 0, GL_RGB,
                     GL_UNSIGNED_BYTE, m_image.bits());
        glErrorCheck();

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glErrorCheck();
    }

    glUseProgram(0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE0);
}

void Skybox::Render(glm::mat4 mvp, GLuint shader_program, GLuint vbo, GLuint vao, glm::vec4 sun_color)
{
    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);
    glErrorCheck();

    glActiveTexture(m_slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
    glUseProgram(shader_program);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glErrorCheck();

    glUniform1i(glGetUniformLocation(shader_program, "cube_tex"), m_slot - GL_TEXTURE0);

    glUniformMatrix4fv(glGetUniformLocation(shader_program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
    glErrorCheck();

    glUniform4fv(glGetUniformLocation(shader_program, "sky_color"), 1, &sun_color[0]);
    glErrorCheck();

    glDrawArrays(GL_TRIANGLES, 0, skybox_verts);
    glErrorCheck();

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUseProgram(0);

    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);

}
