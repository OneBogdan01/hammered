// space 3 is used in fragment shaders by SDL_GPU for uniform buffers as convention
cbuffer Frame : register(b0, space3)
{
    float2 res;
    float time;
    uint count;
    float4 bg_color;
    uint tiles_x;
    uint tile_size;
    uint mode; // 0 = brute force, 1 = tiled, 2 = heatmap
};

struct UICommand
{
    float4 data;
    uint color;
    float shadow_strength;
    uint type;
    uint temp;
};

StructuredBuffer<UICommand> commands : register(t0, space2);
StructuredBuffer<uint> tile_counts : register(t1, space2);
StructuredBuffer<uint> tile_indices : register(t2, space2);

static const uint MAX_ENTRIES_PER_TILE = 200u; // match C++ and tile_shapes.comp

float sdLine(float2 p, float2 a, float2 b, float r)
{
    float2 line_vec = float2(b - a);
    float2 to_point_vec = float2(p - a);
    float h = min(1.0, max(0.0, dot(to_point_vec, line_vec) / dot(line_vec, line_vec)));
    return length(to_point_vec - line_vec * h) - r;
}

float sdBox(float2 p, float2 b)
{
    float2 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float sdCircle(float2 p, float b)
{
    return length(p) - b;
}

void shade(inout float4 color, float2 pixel, uint i)
{
    UICommand cmd = commands[i];
    float d = 1e9;
    if (cmd.type == 0u)
        d = sdCircle(pixel - cmd.data.xy, cmd.data.z);
    else if (cmd.type == 1u)
        d = sdLine(pixel, cmd.data.xy, cmd.data.zw, 2.0);
    else if (cmd.type == 2u)
    {
        float2 center = cmd.data.xy + 0.5 * cmd.data.zw;
        d = sdBox(pixel - center, 0.5 * cmd.data.zw);
    }

    uint c = cmd.color;
    float4 col = float4(
        ((c >> 0) & 0xFF) / 255.0,
        ((c >> 8) & 0xFF) / 255.0,
        ((c >> 16) & 0xFF) / 255.0,
        ((c >> 24) & 0xFF) / 255.0
    );
    color = lerp(color, col, clamp(1.0 - d, 0.0, 1.0) * col.a);
}

float4 main(float4 screenSpace : SV_Position) : SV_Target
{
    float2 pixel = screenSpace.xy;
    float4 color = bg_color;
    uint2 tile = uint2(pixel) / tile_size;
    uint tile_id = tile.y * tiles_x + tile.x;

    if (mode == 0u)
    {
        // brute force: every shape, every pixel
        for (uint i = 0u; i < count; ++i)
            shade(color, pixel, i);
    }
    else if (mode == 1u)
    {
        uint n = min(tile_counts[tile_id], MAX_ENTRIES_PER_TILE);
        for (uint j = 0u; j < n; ++j)
            shade(color, pixel, tile_indices[tile_id * MAX_ENTRIES_PER_TILE + j]);
    }
    else
    {
        // heatmap: stepped bands
        uint n = tile_counts[tile_id];
        float3 hot;
        float mix = 0.7;
        if (n == 0u)
        {
            hot = color.rgb;
            mix = 0.0;
        }
        else if (n <= 2u)
            hot = float3(0.0, 0.4, 1.0);
        else if (n <= 8u)
            hot = float3(0.0, 1.0, 0.4);
        else if (n <= 32u)
            hot = float3(1.0, 0.9, 0.0);
        else if (n <= 128u)
            hot = float3(1.0, 0.45, 0.0);
        else
            hot = float3(1.0, 0.0, 0.0);
        color.rgb = lerp(color.rgb, hot, mix);
    }

    float2 g = frac(pixel / float(tile_size));
    float grid = step(0.9, max(g.x, g.y));
    color.rgb = lerp(color.rgb, float3(0.25, 0.25, 0.3), grid * 0.4);
    return color;
}