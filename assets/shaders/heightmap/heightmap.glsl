#version 450 core

layout (local_size_x = 32, local_size_y = 32) in;
layout (rgba8, binding = 0) uniform image2D heightmap;

#include noise.glsl 

uniform float lacunarity;
uniform float persistance;
uniform int octaves;
uniform float noise_scale;
uniform float exponent;
uniform vec2 offsets[16];

float fbm(vec2 coordinate)
{
    coordinate = coordinate * 2.0 - 1.0;
    // Initial values
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float weights = 0.0;

    // Loop of octaves
    for (int i = 0; i < octaves; i++)
    {
        vec2 sample_coordinates = (frequency * noise_scale * coordinate) + (frequency * offsets[i]);
        value += amplitude * (0.5 + 0.5 * snoise(sample_coordinates));
        weights += amplitude;
        frequency *= lacunarity;
        amplitude *= persistance;
    }

    float height = value / weights;
    return pow(height, exponent);
}

void main()
{
    ivec2 texel_coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 spatial_coordinate = gl_GlobalInvocationID.xy / 2048.0;
    float height = fbm(spatial_coordinate);
    vec4 value = vec4(height);
    imageStore(heightmap, texel_coord, value);
}