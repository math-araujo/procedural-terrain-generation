#version 450

layout (local_size_x = 32, local_size_y = 32) in;

layout (binding=0, rgba8) uniform image2D heightmap;
layout (binding=1, rgba8) uniform image2D normalmap;

shared float local_neighborhood[gl_WorkGroupSize.x+2][gl_WorkGroupSize.y+2];

void sobel_operator()
{
    uvec2 local_pixel = gl_LocalInvocationID.xy + uvec2(1,1);

    float top_right = local_neighborhood[local_pixel.x + 1][local_pixel.y + 1];
    float center_right = local_neighborhood[local_pixel.x + 1][local_pixel.y];
    float bottom_right = local_neighborhood[local_pixel.x + 1][local_pixel.y - 1];
    float top = local_neighborhood[local_pixel.x][local_pixel.y + 1];
    float bottom = local_neighborhood[local_pixel.x][local_pixel.y - 1];
    float top_left = local_neighborhood[local_pixel.x - 1][local_pixel.y + 1];
    float center_left = local_neighborhood[local_pixel.x - 1][local_pixel.y];
    float bottom_left = local_neighborhood[local_pixel.x - 1][local_pixel.y - 1];

    float dx = (top_right + 2 * center_right + bottom_right) - (top_left + 2 * center_left + bottom_left);
    float dy = (bottom_left + 2 * bottom + bottom_right)  - (top_left + 2 * top + top_right);
    vec3 normal = normalize(vec3(dx, dy, 0.1));
    vec3 rgb_normal = (normal + 1.0) / 2.0;
    imageStore(normalmap, ivec2(gl_GlobalInvocationID.xy), vec4(rgb_normal, 1.0));
}

void main()
{
    uvec2 local_pixel = gl_LocalInvocationID.xy + uvec2(1, 1);
    ivec2 global_pixel = ivec2(gl_GlobalInvocationID.xy);

    // Store the current pixel on shared memory
    local_neighborhood[local_pixel.x][local_pixel.y] = imageLoad(heightmap, ivec2(global_pixel.x, global_pixel.y)).r;

    ivec2 local_size = ivec2(gl_WorkGroupSize.xy);
    ivec2 image_size = ivec2(gl_WorkGroupSize.xy * gl_NumWorkGroups.xy);
    if (local_pixel.x == 1) // left-block-edge
    {
        ivec2 left_global_pixel = clamp(global_pixel + ivec2(-1, 0), ivec2(0), image_size - 1);
        local_neighborhood[local_pixel.x - 1][local_pixel.y] = imageLoad(heightmap, left_global_pixel).r;

        if (local_pixel.y == 1) // bottom-left-corner requires to save the diagonal value too
        {
            ivec2 bottom_left_global_pixel = clamp(global_pixel + ivec2(-1, -1), ivec2(0), image_size - 1);
            local_neighborhood[local_pixel.x - 1][local_pixel.y - 1] = imageLoad(heightmap, bottom_left_global_pixel).r;
        }
        else if (local_pixel.y == local_size.y) // top-left-corner
        {
            ivec2 top_left_global_pixel = clamp(global_pixel + ivec2(-1, 1), ivec2(0), image_size - 1);
            local_neighborhood[local_pixel.x - 1][local_pixel.y + 1] = imageLoad(heightmap, top_left_global_pixel).r;
        }
    }

    if (local_pixel.x == local_size.x) // right-block-edge
    {
        ivec2 right_global_pixel = clamp(global_pixel + ivec2(1, 0), ivec2(0), image_size - 1);
        local_neighborhood[local_pixel.x + 1][local_pixel.y] = imageLoad(heightmap, right_global_pixel).r;
        
        if (local_pixel.y == 1) // bottom-right-corner
        {
            ivec2 bottom_right_global_pixel = clamp(global_pixel + ivec2(1, -1), ivec2(0), image_size - 1);
            local_neighborhood[local_pixel.x + 1][local_pixel.y - 1] = imageLoad(heightmap, bottom_right_global_pixel).r;
        }
        else if (local_pixel.y == local_size.y) // top-right corner
        {
            ivec2 top_right_global_pixel = clamp(global_pixel + ivec2(1, 1), ivec2(0), image_size - 1);
            local_neighborhood[local_pixel.x + 1][local_pixel.y + 1] = imageLoad(heightmap, top_right_global_pixel).r;
        }
    }

    if (local_pixel.y == 1) // bottom-block-edge
    {
        ivec2 bottom_global_pixel = clamp(global_pixel + ivec2(0, -1), ivec2(0), image_size - 1);
        local_neighborhood[local_pixel.x][local_pixel.y - 1] = imageLoad(heightmap, bottom_global_pixel).r;
    }

    if (local_pixel.y == local_size.y) // top-block-edge
    {
        ivec2 top_global_pixel = clamp(global_pixel + ivec2(0, 1), ivec2(0), image_size - 1);
        local_neighborhood[local_pixel.x][local_pixel.y + 1] = imageLoad(heightmap, top_global_pixel).r;
    }

    barrier();

    sobel_operator();
}
