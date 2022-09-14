#version 450 core

in vec4 clip_space_position;
in vec2 vertex_tex_coordinates;
in vec3 vertex_to_camera_vector;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D reflection_texture;
layout (binding = 1) uniform sampler2D refraction_texture;
layout (binding = 2) uniform sampler2D dudv_map;

const float wave_strength = 0.01;

uniform float dudv_offset;

void main()
{
    vec2 ndc_position = clip_space_position.xy / clip_space_position.w;
    vec2 screen_coordinates = (ndc_position / 2.0) + 0.5;
    vec2 reflection_tex_coordinates = vec2(screen_coordinates.x, -screen_coordinates.y);
    vec2 refraction_tex_coordinates = screen_coordinates;

    vec2 horizontal_distortion = 2.0 * (texture(dudv_map, vec2(vertex_tex_coordinates.x + dudv_offset, vertex_tex_coordinates.y)).rg) - 1.0;
    vec2 vertical_distortion = 2.0 * (texture(dudv_map, vec2(-vertex_tex_coordinates.x + dudv_offset, vertex_tex_coordinates.y + dudv_offset)).rg) - 1.0;
    vec2 total_distortion = wave_strength * (horizontal_distortion + vertical_distortion);

    reflection_tex_coordinates += total_distortion;
    refraction_tex_coordinates += total_distortion;
    refraction_tex_coordinates = clamp(refraction_tex_coordinates, 0.001, 0.999);
    reflection_tex_coordinates.x = clamp(reflection_tex_coordinates.x, 0.001, 0.999);
    reflection_tex_coordinates.y = clamp(reflection_tex_coordinates.y, -0.999, -0.001);

    vec4 reflection_color = texture(reflection_texture, reflection_tex_coordinates);
    vec4 refraction_color = texture(refraction_texture, refraction_tex_coordinates);
    vec3 view_vector = normalize(vertex_to_camera_vector);
    vec3 water_normal = vec3(0.0, 1.0, 0.0);
    float refraction_factor = clamp(dot(view_vector, water_normal), 0.0, 1.0);
    frag_color = mix(reflection_color, refraction_color, refraction_factor);
}