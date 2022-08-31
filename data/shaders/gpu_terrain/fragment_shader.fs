#version 450 core

in float tes_height;
in vec3 tes_color;
in vec2 tes_tex_coords;
in vec3 frag_pos;

out vec4 frag_color;

struct Light
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

uniform Light light;
uniform sampler2D normal_map_sampler;

void main()
{
    float h = (tes_height)/ 15.0;
    vec4 color = vec4(tes_color, 1.0);
    
    // Ambient Light Component
    vec3 ambient_color = light.ambient * color.rgb;

    // Diffuse Light Component    
    vec3 normal_rgb = texture(normal_map_sampler, tes_tex_coords).rgb;
    vec3 normal = normal_rgb * 2.0 - 1.0;
    normal = normal.rbg; // Swap B and G, because our height is on Y-axis, not Z-axis
    vec3 unit_normal = normalize(normal);

    vec3 light_dir = normalize(-light.direction);
    float diffuse_intensity = max(dot(unit_normal, light_dir), 0.0);
    vec3 diffuse_color = light.diffuse * diffuse_intensity * color.rgb;

    vec3 result = ambient_color + diffuse_color;
    frag_color = vec4(result, 1.0);
}