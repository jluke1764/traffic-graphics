#version 330 core
in vec3 coord;

out vec4 fragColor;

uniform samplerCube cube_tex;

void main()
{
    fragColor = texture(cube_tex, coord);
}
