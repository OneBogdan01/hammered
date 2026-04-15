#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

layout(location = 0) out vec2 TexCoords;

#ifdef VULKAN
#define gl_VertexID gl_VertexIndex
#endif

void main()
{
    TexCoords = aTexCoords;
    vec4 pos = vec4(aPos, 1.0);

#ifndef VULKAN
    pos.y = -pos.y;
#endif

    gl_Position = pos;
}