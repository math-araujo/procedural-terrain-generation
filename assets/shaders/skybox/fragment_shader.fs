#version 450 core

in vec3 vertex_tex_coordinates;

out vec4 frag_color;

layout (binding = 0) uniform samplerCube skybox;

void main()
{
    frag_color = texture(skybox, vertex_tex_coordinates);
}