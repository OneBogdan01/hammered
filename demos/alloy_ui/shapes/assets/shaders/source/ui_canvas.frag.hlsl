//space 3 is used in fragment shaders by SDL_GPU for uniform buffers as convention
cbuffer Frame: register(b0, space3)
{
    float2 res;
    float time;
};
struct UICommand {
    float4 data;      // packed shape data
    uint   color;     // packed rgba8
    float  shadow_strength;
    uint   type;
    uint   temp;
};

StructuredBuffer<UICommand> commands : register(t0, space2);
float sdLine(float2 p, float2 a, float2 b, float r)
{
float2 line_vec=float2(b-a);
float2 to_point_vec = float2(p-a);
float h = min(1.0,max(0.0,dot(to_point_vec,line_vec)/dot(line_vec,line_vec)));



return length(to_point_vec-line_vec*h)-r;
}

float sdBox( float2 p, float2 b )
{
    float2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}
float sdCircle( float2 p, float b )
{
    return length(p)-b;
   
}
float4 main(float4 screenSpace : SV_Position ) : SV_Target
{

     float  aspect = res.x / res.y;
    float2 uv     = (screenSpace.xy * 2.0 - res) / res.y;
    
    float2 rect = float2(.2,.5);
    float d= sdBox(uv,rect)-0.2;
  	float2 circlePos = float2(1., 0.5);
  	float2 circlePos2 = float2(-1., 0.5);
	d = min(d,sdCircle(uv - circlePos2,.5));
	d = min(d,sdCircle(uv - circlePos,.5));
	d = min(d,sdLine(uv+0.6,circlePos2-0.5,circlePos,.1));	
    float3 col = (d>0.0) ? float3(1.,0.0,0.2) : float3(0.3,0.5,.6);
    col *= 1.0 - exp2(-20.0*abs(d));
	col *= 0.9 + 1.0*cos(120.0*abs(d)+time);
	col = lerp( col, float3(0.0,0.0,0.9), 1.0-smoothstep(0.0,0.01,abs(d)) );
    float alpha = 1.0 - smoothstep(0.0, 0.9, d);
    return float4(col, 1)* alpha;

}
