#version 450 core

layout (location = 0) in vec3 input_position;
layout (location = 1) in vec2 input_tex_coordinates;

out vec2 vertex_tex_coordinates;

void main()
{
    gl_Position = vec4(input_position, 1.0);
    vertex_tex_coordinates = input_tex_coordinates;
}