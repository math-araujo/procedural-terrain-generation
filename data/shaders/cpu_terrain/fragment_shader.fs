#version 450 core

in vec2 tex_coordinates;

out vec4 frag_color;

uniform sampler2D texture_sampler;

void main()
{
    frag_color = vec4(texture(texture_sampler, tex_coordinates).rgb, 1.0);
}