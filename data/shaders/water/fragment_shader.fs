#version 450 core

in vec4 clip_space_position;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D reflection_texture;
layout (binding = 1) uniform sampler2D refraction_texture;

void main()
{
    vec2 ndc_position = clip_space_position.xy / clip_space_position.w;
    vec2 screen_coordinates = (ndc_position / 2.0) + 0.5;
    vec2 reflection_tex_coordinates = vec2(screen_coordinates.x, 1.0 -screen_coordinates.y);

    vec4 reflection_color = texture(reflection_texture, reflection_tex_coordinates);
    vec4 refraction_color = texture(refraction_texture, screen_coordinates);
    frag_color = mix(reflection_color, refraction_color, 0.5);
}