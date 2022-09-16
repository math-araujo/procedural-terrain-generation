#version 450 core
layout (location = 0) in vec3 input_vertex_position;

out vec3 vertex_tex_coordinates;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vertex_tex_coordinates = input_vertex_position;
    gl_Position = (projection * view * vec4(input_vertex_position, 1.0)).xyww;
}