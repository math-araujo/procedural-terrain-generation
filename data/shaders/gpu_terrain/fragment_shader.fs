#version 450 core

in float tes_height;
in vec3 tes_color;

out vec4 frag_color;

void main()
{
    float h = tes_height / 15.0;
    frag_color = vec4(tes_color, 1.0);
}