//space 3 is used in fragment shaders by SDL_GPU for uniform buffers as convention
cbuffer Frame : register(b0, space3)
{
    float2 res;
    float time;
    uint count;
    float4 bg_color;
};
struct UICommand
{
    float4 data; // packed shape data
    uint color; // packed rgba8
    float shadow_strength;
    uint type;
    uint temp;
};

StructuredBuffer<UICommand> commands : register(t0, space2);
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
float4 main(float4 screenSpace : SV_Position) : SV_Target
{
    float2 pixel = screenSpace.xy;
    float4 color = bg_color;
    for (uint i = 0u; i < count; ++i)
    {
        UICommand cmd = commands[i];

        float d = 1e9;
        if (cmd.type == 0u)
        { // Circle
            float2 center = cmd.data.xy;
            float radius = cmd.data.z;
            d = sdCircle(pixel - center, radius);
        }
        else if (cmd.type == 1u)
        { // Line
            float2 a = cmd.data.xy;
            float2 b = cmd.data.zw;
            d = sdLine(pixel, a, b, 2.0);
        }
        else if (cmd.type == 2u)
        { // Rect
            float2 rect_pos = cmd.data.xy;
            float2 rect_size = cmd.data.zw;
            float2 center = rect_pos + 0.5 * rect_size;
            float2 half_size = 0.5 * rect_size;
            d = sdBox(pixel - center, half_size);
        }
    
        // Unpack the RGBA8 color.
        uint c = cmd.color;
        float4 col = float4(
            ((c >> 0) & 0xFF) / 255.0,
            ((c >> 8) & 0xFF) / 255.0,
            ((c >> 16) & 0xFF) / 255.0,
            ((c >> 24) & 0xFF) / 255.0
        );
        
        
        color = lerp(color, col, clamp(1.0 - d, 0.0, 1.0)*col.a);
        
       
    }
    return color;

}
    //float aspect = res.x / res.y;
    //float2 uv = (screenSpace.xy * 2.0 - res) / res.y;
    //float2 rect = float2(.2, .5);
    //float d = sdBox(uv, rect) - 0.2;
    //float2 circlePos = float2(1., 0.5);
    //float2 circlePos2 = float2(-1., 0.5);
    //d = min(d, sdCircle(uv - circlePos2, .5));
    //d = min(d, sdCircle(uv - circlePos, .5));
    //d = min(d, sdLine(uv + 0.6, circlePos2 - 0.5, circlePos, .1));
    //float3 col = (d > 0.0) ? float3(1., 0.0, 0.2) : float3(0.3, 0.5, .6);
    //col *= 1.0 - exp2(-20.0 * abs(d));
    //col *= 0.9 + 1.0 * cos(120.0 * abs(d) + time);
    //col = lerp(col, float3(0.0, 0.0, 0.9), 1.0 - smoothstep(0.0, 0.01, abs(d)));
    //float alpha = 1.0 - smoothstep(0.0, 0.9, d);