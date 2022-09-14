#version 450 core

in float tes_height;
in vec2 tes_tex_coords;
in vec3 tes_frag_pos;
in vec3 tes_normal;

out vec4 frag_color;

struct Light
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform bool use_triplanar_texturing;

const int size = 5;
uniform sampler2D albedos[size];
//Water Sand Grass Rock Snow
                                                   // Water Sand Grass Rock Snow
const float start_heights[size + 1] = float[size + 1](0.0, 0.3, 0.35, 0.5, 0.85, 1.1);
                                      // Water  Sand Grass Rock Snow
const float blend_end[size] = float[size](0.32, 0.4, 0.6, 0.9, 1.1);

const float triplanar_scale = 10.56;

vec4 triplanar_texture_mapping(vec3 frag_normal, sampler2D sampler)
{
    vec3 abs_normal = abs(frag_normal);
    abs_normal = normalize(max(abs_normal, 0.00001));
    float sum = abs_normal.x + abs_normal.y + abs_normal.z;
    abs_normal /= sum;
    
    vec3 x_axis = texture(sampler, tes_frag_pos.yz / triplanar_scale).rgb;
    vec3 y_axis = texture(sampler, tes_frag_pos.xz / triplanar_scale).rgb;
    vec3 z_axis = texture(sampler, tes_frag_pos.xy / triplanar_scale).rgb;
    return vec4(x_axis * abs_normal.x + y_axis * abs_normal.y + z_axis * abs_normal.z, 1.0);
}

void main()
{
    float height = tes_height;
    vec4 color = vec4(1.0);
    
    // Compute fragment normal
    vec3 unit_normal = normalize(tes_normal);
    float slope = 1.0 - unit_normal.y; // slope == 0.0 -> flat plane; slope == 1.0 -> vertical plane

    vec4 colors[size];
    for (int i = 0; i < size; ++i)
    {
        if (use_triplanar_texturing)
        {
            colors[i] = triplanar_texture_mapping(unit_normal, albedos[i]);
        }
        else
        {
            colors[i] = texture(albedos[i], tes_frag_pos.xz / triplanar_scale);;
        }
    }

    if (height < start_heights[1])
    {
        color = colors[0];
    }
    else
    {
        for (int i = 1; i < size; ++i)
        {
            if (height >= start_heights[i] && height < start_heights[i + 1])
            {
                float param = smoothstep(start_heights[i], blend_end[i - 1], height);
                color = mix(colors[i - 1], colors[i], param);
                break;
            }
        }
    }

    if (slope >= 0.15 && height > start_heights[1])
    {
        float param = smoothstep(0.15, 0.3, slope);
        color = mix(color, colors[3], param);
    }
    
    // Ambient Light Component
    vec3 ambient_color = light.ambient * color.rgb;

    // Diffuse Light Component    
    vec3 light_dir = normalize(-light.direction);
    float diffuse_intensity = max(dot(unit_normal, light_dir), 0.0);
    vec3 diffuse_color = light.diffuse * diffuse_intensity * color.rgb;

    vec3 result = ambient_color + diffuse_color;
    frag_color = vec4(result, 1.0);
}