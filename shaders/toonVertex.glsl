#version 410 core
in vec3 inVert;
layout (location = 2) in vec3 inNormal;
in vec2 inUV;

uniform vec3 viewerPos;
uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalMatrix;
//uniform mat4 M;
uniform float scale;


smooth out float intense;
out vec3 fragmentNormal;
out float val;
out vec3 position;
out vec2 vertUV;

// @brief light structure
struct Lights
{
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constantAttenuation;
    float spotCosCutoff;
    float quadraticAttenuation;
    float linearAttenuation;
};

uniform Lights light;

//@brief get the light intensity
float intensity(vec3 lightD,vec3 normals)
{
    float result= dot(lightD,normals);
    return result;
}

void main ()
{
    // calculate the fragmentNormal, light intensity as well as the vertex UV coordinates to be passed into fragmentshader
    fragmentNormal =  (normalMatrix* inNormal);
    vec3 tnorm = normalize (normalMatrix * inNormal);
    vec4 eyeCord=MV*vec4(inVert,1);
    vec3 lightDir=vec3(light.position.xyz-eyeCord.xyz);
    intense=intensity(lightDir,tnorm.xyz);
    vec4 pos =vec4(inVert*scale,1.0f);
    vec3 position = inVert.xyz;
    gl_Position = MVP*pos;
    vertUV=inUV.st;

}
