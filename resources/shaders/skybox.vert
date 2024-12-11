#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 mvp;

out vec3 coord;

void main()
{
    vec4 proj_pos = mvp * vec4(pos, 1.0);
    gl_Position = proj_pos.xyww;
    coord = normalize(pos);
}
