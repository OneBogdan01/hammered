// already uploaded to GPU memory for the fragment shader
struct UICommand
{
    float4 data;
    uint color;
    float shadow_strength;
    uint type;
    uint temp;
};

StructuredBuffer<UICommand> commands : register(t0, space0);

RWStructuredBuffer<uint> counts : register(u0, space1);  // one per tile
RWStructuredBuffer<uint> indices : register(u1, space1); // tile_count * MAX

cbuffer TileParams : register(b0, space2)
{
    uint tiles_x;
    uint tiles_y;
    uint tile_size;
    uint count;
};

static const uint MAX_ENTRIES_PER_TILE = 200u;
static const float PADDING = 3.0;

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    // i is the shape index
    uint i = id.x;
    if (i >= count) return;

    UICommand cmd = commands[i];

    float2 lo, hi;
    if (cmd.type == 0u) { // circle
        lo = cmd.data.xy - cmd.data.z;
        hi = cmd.data.xy + cmd.data.z;
    } else if (cmd.type == 1u) { // line
        lo = min(cmd.data.xy, cmd.data.zw);
        hi = max(cmd.data.xy, cmd.data.zw);
    } else { // rect
        lo = cmd.data.xy;
        hi = cmd.data.xy + cmd.data.zw;
    }
    lo -= PADDING;
    hi += PADDING;

    // clamp to screen
    int2 lim = int2(int(tiles_x) - 1, int(tiles_y) - 1);
    int2 tmin = clamp(int2(floor(lo)) / int(tile_size), int2(0, 0), lim);
    int2 tmax = clamp(int2(floor(hi)) / int(tile_size), int2(0, 0), lim);

    for (int ty = tmin.y; ty <= tmax.y; ++ty) {
        for (int tx = tmin.x; tx <= tmax.x; ++tx) {
            uint tile_id = uint(ty) * tiles_x + uint(tx);
            uint slot;
            InterlockedAdd(counts[tile_id], 1u, slot);
            if (slot < MAX_ENTRIES_PER_TILE) {
                indices[tile_id * MAX_ENTRIES_PER_TILE + slot] = i;
            }
        }
    }
}