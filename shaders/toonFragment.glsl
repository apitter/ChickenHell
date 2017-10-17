#version 410 core
out vec4 fragColour;
smooth in float intense;
in vec3 fragmentNormal;
in float val;
in vec3 position;

in vec2 vertUV;
uniform float dissolveFactor;
uniform sampler2D tex;

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

//@brief tonemapping

vec3 getExposure(in vec3 color)
{
    vec3 retColor;

    color *= 1.1;

    retColor = pow(color,vec3(1/2.2));


    return retColor;
}



vec3 convertToHDR( in vec3 color)
{
    vec3 HDR;

    vec3 overExposed = color *1.2;
    vec3 underExposed = color/1.5;

    HDR = mix(underExposed,overExposed,color);

    return HDR;
}


vec3 Reinhard(in vec3 color)
{
    color = color/(1+color);

    return pow(color,vec3(1/2.2));

}


vec3 Burgess (in vec3 color)

{
    vec3 maxColor = max(vec3(0.0),color-0.004);
    vec3 retColor = (maxColor*(6.2*maxColor+0.5))/(maxColor*(6.2*maxColor +1.7)+0.06);


    return pow(retColor,vec3(1/2.2));

}



void main ()
{
    //get colours to be
    vec4 color;
    if (intense > 0.95)
            color = vec4(1.0,0.8,0.7,1.0);
    else if (intense > 0.5)
            color = vec4(0.6,0.3,0.3,1.0);
    else if (intense > 0.25)
            color = vec4(0.4,0.2,0.2,1.0);
    else
            color = vec4(0.2,0.1,0.1,1.0);


    vec4 Colour;

    float dx = 15*(1./512.);
    float dy = 20.0*(1./512.);
    vec2 coord = vec2(dx*floor(vertUV.x/dx),
                      dy*floor(vertUV.y/dy));

    //Colour = vec4(Burgess(color.xyz),0.7);
    //Colour = vec4(convertToHDR(Colour.xyz),1.0);

    fragColour = color*texture2D(tex,coord);

    //fragColour = color;
}

