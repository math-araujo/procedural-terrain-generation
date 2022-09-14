#version 450 core

layout (location = 0) in vec3 input_vertex_position;
layout (location = 1) in vec2 input_tex_coordinates;

uniform mat4 mvp;
uniform mat4 model;
uniform vec3 camera_position;

out vec4 clip_space_position;
out vec2 vertex_tex_coordinates;
out vec3 vertex_to_camera_vector;

const float tiling = 6.0;

void main()
{
    vec4 vertex_position = vec4(input_vertex_position, 1.0);
    vertex_to_camera_vector = camera_position - (model * vertex_position).xyz;
    clip_space_position = mvp * vertex_position;
    vertex_tex_coordinates = input_tex_coordinates * tiling;
    gl_Position = clip_space_position;
}