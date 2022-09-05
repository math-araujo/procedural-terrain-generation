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

const int size = 5;
uniform sampler2D albedos[size];
//Water Sand Grass Rock Snow
                                                   // Water Sand Grass Rock Snow
const float start_heights[size + 1] = float[size + 1](0.0, 0.3, 0.35, 0.5, 0.85, 1.1);
                                      // Water  Sand Grass Rock Snow
const float blend_end[size] = float[size](0.31, 0.38, 0.7, 0.9, 1.1);

void main()
{
    float h = (tes_height)/ 30.0;
    vec3 color = vec3(1.0);
    
    vec3 colors[size];
    for (int i = 0; i < size; ++i)
    {
        colors[i] = texture(albedos[i], tes_tex_coords).rgb;
    }

    if (h < start_heights[1])
    {
        color = colors[0];
    }
    else
    {
        for (int i = 1; i < size; ++i)
        {
            if (h >= start_heights[i] && h < start_heights[i + 1])
            {
                float param = smoothstep(start_heights[i], blend_end[i - 1], h);
                color = mix(colors[i - 1], colors[i], param);
                break;
            }
        }
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