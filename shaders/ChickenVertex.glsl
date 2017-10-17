#version 330 core

/// @brief MVP passed from app
uniform mat4 MVP;
uniform mat3 normalMatrix;
// first attribute the vertex values from our VAO
layout (location=0)in vec3 inVert;
// second attribute the UV values from our VAO
layout (location=1)in vec2 inUV;

layout (location=2)in vec3 inNormal;
// we use this to pass the UV values to the frag shader
out vec2 vertUV;

out vec3 fragmentNormal;

void main()
{
fragmentNormal = (normalMatrix*inNormal);
// calculate the vertex position
gl_Position = MVP*vec4(inVert, 1.0);
// pass the UV values to the frag shader
vertUV=inUV.st;
}
