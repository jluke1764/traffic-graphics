#version 330 core
in vec3 coord;

out vec4 fragColor;

uniform samplerCube cube_tex;
uniform vec4 sky_color;

void main()
{
    fragColor = texture(cube_tex, coord) * sky_color;
}
