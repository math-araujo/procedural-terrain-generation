#version 450 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_tex_coordinates;

out vec2 tex_coordinates;

uniform mat4 proj_view_transform;

void main()
{
    gl_Position = proj_view_transform * vec4(vertex_position, 1.0);
    tex_coordinates = vertex_tex_coordinates;
}