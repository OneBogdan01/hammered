//space 3 is used in fragment shaders by SDL_GPU for uniform buffers as convention
cbuffer Frame: register(b0, space3)
{
    float2 res;
    float time;
};
float sdBox( float2 p, float2 b )
{
    float2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}
float4 main(float4 screenSpace : SV_Position ) : SV_Target
{

     float  aspect = res.x / res.y;
    float2 uv     = (screenSpace.xy * 2.0 - res) / res.y;
    
    float2 rect = float2(.2,.9);
    float d= sdBox(uv,rect)-0.2;
     float3 col = (d>0.0) ? float3(1.,0.0,0.2) : float3(0.3,0.5,.6);
    col *= 1.0 - exp2(-20.0*abs(d));
	col *= 0.9 + 1.0*cos(120.0*abs(d)+time);
	col = lerp( col, float3(0.0,0.0,0.9), 1.0-smoothstep(0.0,0.01,abs(d)) );
    float alpha = 1.0 - smoothstep(0.0, 0.9, d);
    return float4(col, 1)* alpha;

}
