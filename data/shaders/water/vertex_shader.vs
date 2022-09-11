#version 450 core

layout (location = 0) in vec3 input_vertex_position;
layout (location = 1) in vec2 input_tex_coordinates;

uniform mat4 mvp;

out vec4 clip_space_position;

void main()
{
    clip_space_position = mvp * vec4(input_vertex_position, 1.0);
    gl_Position = clip_space_position;
}