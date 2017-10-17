#version 330 core

/// @brief Model transform matrix passed in from UI Class in main app
uniform mat4 MVP;

layout (location=0) in vec3 inVert;
layout (location=1)in vec2 inUV;
out vec2 vertUV;

void main()
{
    // calculate the vertex position
    gl_Position = MVP*vec4(inVert, 1.0);
    // get the texture co-ord and mutliply it by the texture matrix
    vertUV=inUV.st;
}