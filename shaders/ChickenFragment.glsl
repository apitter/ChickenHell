#version 330 core

// this is a pointer to the current 2D texture object
uniform sampler2D tex;
// the vertex UV
in vec2 vertUV;
in vec3 fragmentNormal;
uniform vec4 Colour;
uniform vec3 lightPos;
uniform vec4 lightDiffuse;

layout (location=0)out vec4 outColour;
void main ()
{
 // set the fragment colour to the current texture
 vec4 fragColour= vec4(0.);
 vec3 N = normalize(fragmentNormal);
 vec3 L = normalize(lightPos);
 fragColour += Colour*lightDiffuse *dot(L, N);
 outColour = fragColour*texture(tex,vertUV);
}
