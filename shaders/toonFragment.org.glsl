#version 410 core
out vec4 fragColour;
smooth in float intense;
in vec3 fragmentNormal;
in float val;
in vec3 position;

in vec2 vertUV;
uniform float dissolveFactor;
uniform sampler2D cloudTexture;

struct Materials
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};

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

uniform Materials material;

uniform Lights light;

void main ()
{

    vec4 color;
    if (intense > 0.95)
            color = vec4(1.0,0.5,0.5,1.0);
    else if (intense > 0.5)
            color = vec4(0.6,0.3,0.3,1.0);
    else if (intense > 0.25)
            color = vec4(0.4,0.2,0.2,1.0);
    else
            color = vec4(0.2,0.1,0.1,1.0);



    //vec2 col = (noise2(position.xy)*0.5)+0.5;

    //float val = (col.x + col.y) / 2.0;
    //vec4 vCloudSample = texture2D(cloudTexture,vertUV);


    fragColour = color;
}

