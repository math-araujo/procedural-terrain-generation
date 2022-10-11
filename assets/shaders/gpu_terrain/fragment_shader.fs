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

struct Fog
{
    float height;
    float density;
};

uniform Light light;
uniform bool use_triplanar_texturing;
uniform vec3 camera_position;
uniform Fog fog;
uniform bool apply_fog;
uniform bool apply_normal_map;

const int size = 3;
uniform float triplanar_scale[size];
uniform float start_heights[size + 1];
uniform float blend_end[size];
layout (binding = 2) uniform sampler2DArray albedos;
layout (binding = 3) uniform sampler2DArray terrain_ao;
layout (binding = 4) uniform sampler2DArray terrain_normal_map;


// Triplanar texture mapping for array texture
vec4 triplanar_texture_mapping(vec3 frag_normal, float triplanar_scale, int texture_index)
{
    vec3 abs_normal = abs(frag_normal);
    abs_normal = normalize(max(abs_normal, 0.00001));
    float sum = abs_normal.x + abs_normal.y + abs_normal.z;
    abs_normal /= sum;
    
    vec3 x_axis = texture(albedos, vec3(tes_frag_pos.yz * triplanar_scale, texture_index)).rgb;
    vec3 y_axis = texture(albedos, vec3(tes_frag_pos.xz * triplanar_scale, texture_index)).rgb;
    vec3 z_axis = texture(albedos, vec3(tes_frag_pos.xy * triplanar_scale, texture_index)).rgb;
    return vec4(x_axis * abs_normal.x + y_axis * abs_normal.y + z_axis * abs_normal.z, 1.0);
}

vec3 triplanar_blending(vec3 frag_normal)
{
    vec3 abs_normal = abs(frag_normal);
    abs_normal = normalize(max(abs_normal, 0.00001));
    float sum = abs_normal.x + abs_normal.y + abs_normal.z;
    abs_normal /= sum;
    return abs_normal;
}

vec3 triplanar_normal_mapping(vec3 triplanar_blending, sampler2DArray sampler, float triplanar_scale, int texture_index)
{    
    vec3 x_axis = 2.0 * texture(sampler, vec3(tes_frag_pos.yz * triplanar_scale, texture_index)).rgb - 1.0;
    vec3 y_axis = 2.0 * texture(sampler, vec3(tes_frag_pos.xz * triplanar_scale, texture_index)).rgb - 1.0;
    vec3 z_axis = 2.0 * texture(sampler, vec3(tes_frag_pos.xy * triplanar_scale, texture_index)).rgb - 1.0;
    return vec3(x_axis * triplanar_blending.x + y_axis * triplanar_blending.y + z_axis * triplanar_blending.z);
}

float triplanar_ao_mapping(vec3 triplanar_blending, sampler2DArray sampler, float triplanar_scale, int texture_index)
{    
    float x_axis = texture(sampler, vec3(tes_frag_pos.yz * triplanar_scale, texture_index)).r;
    float y_axis = texture(sampler, vec3(tes_frag_pos.xz * triplanar_scale, texture_index)).r;
    float z_axis = texture(sampler, vec3(tes_frag_pos.xy * triplanar_scale, texture_index)).r;
    return x_axis * triplanar_blending.x + y_axis * triplanar_blending.y + z_axis * triplanar_blending.z;
}

const vec3 fog_color = vec3(0.75, 0.75, 0.75);
const vec3 halfspace_plane_normal = vec3(0.0, 1.0, 0.0);

float compute_halfspace_fog_factor(vec3 fragment_pos)
{
    vec4 frag_pos = vec4(fragment_pos, 1.0);
    vec4 camera_pos = vec4(camera_position, 1.0);
    vec4 view_vector = camera_pos - frag_pos;
    vec4 F = vec4(halfspace_plane_normal, -fog.height);
    float f_dot_p = dot(F, frag_pos);
    float f_dot_c = dot(F, camera_pos);
    float f_dot_v = dot(F, view_vector);
    float k = clamp(-sign(f_dot_c), 0.0, 1.0); // k = 1 if dot(F, C) < 0, otherwise k = 0
    vec3 aV = (fog.density / 2) * vec3(view_vector);
    float c1 = k * (f_dot_p + f_dot_c);
    float c2 = (1.0 - 2.0 * k) * f_dot_p;
    float g = min(c2, 0.0);
    const float epsilon = 0.0001;
    g = -length(aV) * (c1 - g * g / (abs(f_dot_v) + epsilon));
    
    return 1.0 - clamp(exp2(-(g*g)), 0.0, 1.0);
}

mat3 TBN;

void get_tbn()
{
    vec3 Q1  = dFdx(tes_frag_pos);
    vec3 Q2  = dFdy(tes_frag_pos);
    vec2 st1 = dFdx(tes_tex_coords);
    vec2 st2 = dFdy(tes_tex_coords);
    
    vec3 N   = normalize(tes_normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    TBN = mat3(T, B, N);
}

void main()
{
    float height = tes_height;
    
    // Compute fragment normal
    get_tbn();
    vec3 world_space_unit_normal = normalize(tes_normal);

    vec4 colors[size];
    float aos[size];
    vec3 normals[size];
    vec3 triplanar_blending_factor = triplanar_blending(world_space_unit_normal);
    for (int i = 0; i < size; ++i)
    {
        if (use_triplanar_texturing)
        {
            vec3 albedo = pow(triplanar_texture_mapping(world_space_unit_normal, triplanar_scale[i], i).rgb, vec3(2.2));
            colors[i] = vec4(albedo, 1.0);
            aos[i] = triplanar_ao_mapping(triplanar_blending_factor, terrain_ao, triplanar_scale[i], i);
            vec3 tangent_normal = triplanar_normal_mapping(triplanar_blending_factor, terrain_normal_map, triplanar_scale[i], i);
            normals[i] = normalize(TBN * tangent_normal);
        }
        else
        {
            vec3 albedo = pow(texture(albedos, vec3(tes_frag_pos.xz * triplanar_scale[i], i)).rgb, vec3(2.2));
            colors[i] = vec4(albedo, 1.0);
            aos[i] = texture(terrain_ao,  vec3(tes_frag_pos.xz * triplanar_scale[i], i)).r;
            vec3 tangent_normal = texture(terrain_normal_map,  vec3(tes_frag_pos.xz * triplanar_scale[i], i)).rgb;
            tangent_normal = tangent_normal * 2.0 - 1.0;
            normals[i] = normalize(TBN * tangent_normal);
        }
    }

    vec4 color = vec4(1.0);
    float ao = 0.0;
    vec3 unit_normal = world_space_unit_normal;
    if (height < start_heights[1])
    {
        color = colors[0];
        ao = aos[0];
        if (apply_normal_map)
        {
            unit_normal = normals[0];
        }
    }
    else
    {
        for (int i = 1; i < size; ++i)
        {
            if (height >= start_heights[i] && height < start_heights[i + 1])
            {
                float param = smoothstep(start_heights[i], blend_end[i - 1], height);
                color = mix(colors[i - 1], colors[i], param);
                ao = mix(aos[i - 1], aos[i], param);
                if (apply_normal_map)
                {
                    unit_normal = normalize(mix(normals[i - 1], normals[i], param));
                }
                break;
            }
        }
    }

    float slope = 1.0 - world_space_unit_normal.y; // slope == 0.0 -> flat plane; slope == 1.0 -> vertical plane
    if (slope > 0.15 && height >= start_heights[2])
    {
        float param = smoothstep(0.15, 0.8, slope);
        color = mix(color, colors[1], param);
    }
    
    // Ambient Light Component
    vec3 ambient_color;
    ambient_color = vec3(0.1) * color.rgb * ao;

    // Diffuse Light Component    
    vec3 light_dir = normalize(-light.direction);
    float diffuse_intensity = max(dot(unit_normal, light_dir), 0.0);
    vec3 diffuse_color = light.diffuse * diffuse_intensity * color.rgb;

    vec3 result = ambient_color + diffuse_color;
    frag_color = vec4(pow(result, vec3(1.0/2.2)), 1.0);

    // Add Fog
    if (apply_fog)
    {
        float fog_factor = compute_halfspace_fog_factor(tes_frag_pos);
        frag_color = mix(frag_color, vec4(fog_color, 1.0), fog_factor);
        //frag_color = mix(frag_color, vec4(fog_factor, fog_factor, fog_factor, 1.0), 0.97); // debug fog factor
    }
    //frag_color = mix(frag_color, vec4(unit_normal, 1.0), 0.98); // debug normal map
    //frag_color = mix(frag_color, vec4(world_space_unit_normal, 1.0), 0.98); // debug world normal
}