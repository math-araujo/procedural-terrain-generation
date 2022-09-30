#version 450 core

layout (local_size_x = 32, local_size_y = 32) in;
layout(rgba8, binding = 0) uniform image2D heightmap;

#include noise.glsl 

uniform float exponent;
uniform float random_offsets_x[16];
uniform float random_offsets_y[16];
uniform float lacunarity;
uniform float persistance;
uniform float noise_scale;
uniform float octaves;

// The functions to compute random numbers and random vectors on GLSL
// are due to the user Spatial on StackOverflow on the following answer:
// Ref.: https://stackoverflow.com/a/17479300

uint hash(uint x)
{
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash(uvec2 v)
{
    return hash(v.x ^ hash(v.y)); 
}

uint hash(uvec3 v)
{
    return hash(v.x ^ hash(v.y) ^ hash(v.z)); 
}

uint hash(uvec4 v)
{ 
    return hash(v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w)); 
}

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float float_construct(uint m)
{
    const uint ieee_mantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieee_one      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieee_mantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieee_one;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random(float x)
{
    return float_construct(hash(floatBitsToUint(x)));
}

float random(vec2 v)
{
    return float_construct(hash(floatBitsToUint(v)));
}

float random(vec3 v)
{
    return float_construct(hash(floatBitsToUint(v)));
}

float random(vec4  v)
{
    return float_construct(hash(floatBitsToUint(v)));
}

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
        value += amplitude * (0.5 + 0.5 * snoise(frequency * noise_scale * coordinate));
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

	vec3 color = vec3(0.0);
	color += fbm(spatial_coordinate);
	vec4 value = vec4(color, color.x);
    
    // Visualization of extreme values
    /*
    if (color.x > 0.8)
    {
        value = vec4(1.0, 0.0, 0.0, color.x);
    }
    else if (color.x < 0.2)
    {
        value = vec4(0.0, 1.0, 0.0, color.x);
    }*/
    imageStore(heightmap, texel_coord, value);
}