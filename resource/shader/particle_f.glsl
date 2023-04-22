#version 330 core
out vec4 FragColor;

uniform vec4 particle_color;

void main()
{
    FragColor = particle_color;
}
