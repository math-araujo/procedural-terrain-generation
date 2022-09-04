#version 450 core

in float tes_height;
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

uniform sampler2D water_sampler;
uniform sampler2D sand_sampler;
uniform sampler2D grass_sampler;
uniform sampler2D rock_sampler;
uniform sampler2D snow_sampler;

void main()
{
    float h = (tes_height)/ 30.0;
    vec3 color = vec3(1.0);
    
    vec3 water_color = texture(water_sampler, tes_tex_coords).rgb;
    vec3 sand_color = texture(sand_sampler, tes_tex_coords).rgb;
    vec3 grass_color = texture(grass_sampler, tes_tex_coords).rgb;
    vec3 rock_color = texture(rock_sampler, tes_tex_coords).rgb;
    vec3 snow_color = texture(snow_sampler, tes_tex_coords).rgb;
    
    if (h < 0.3)
    {
        color = water_color;
    }
    else if (h < 0.35)
    {
        float param = smoothstep(0.3, 0.35, h);
        color = mix(water_color, sand_color, param);
    }
    else if (h < 0.4)
    {
        float param = smoothstep(0.35, 0.4, h);
        color = mix(sand_color, grass_color, param);
    }
    else if (h < 0.85)
    {
        float param = smoothstep(0.4, 0.85, h);
        color = mix(grass_color, rock_color, param);
    }
    else if (h < 0.9)
    {
        float param = smoothstep(0.85, 0.9, h);
        color = mix(rock_color, snow_color, param);
    }
    else
    {
        color = snow_color;
    }
    
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