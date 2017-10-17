#version 410 core
/// @brief our output fragment colour

uniform sampler2D tex;
in vec2 vertUV;

layout (location =0) out vec4 fragColour;
/// @brief[in] the vertex normal
in  vec3 fragmentNormal;
/// @brief material structure
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
// @param material passed from our program
uniform Materials material;

uniform Lights light;
in vec3 lightDir;
// out the blinn half vector
in vec3 halfVector;
in vec3 eyeDirection;
in vec3 vPosition;

/// @brief a function to compute point light values
/// @param[in] _light the number of the current light

vec4 pointLight()
{
  vec3 N = normalize(fragmentNormal);
  vec3 halfV;
  float ndothv;
  float attenuation;
  vec3 E = normalize(eyeDirection);
  vec3 L = normalize(lightDir);
  float lambertTerm = dot(N,L);
  vec4 diffuse=vec4(0);
  vec4 ambient=vec4(0);
  vec4 specular=vec4(0);
  if (lambertTerm > 0.0)
  {
  float d;            // distance from surface to light position
  vec3 VP;            // direction from surface to light position

  // Compute vector from surface to light position
  VP = vec3 (light.position) - vPosition;

  // Compute distance between surface and light position
    d = length (VP);

  //combine the lighting model with textures
    diffuse+=texture2D(tex, vertUV)*light.diffuse*lambertTerm;
    ambient+=material.ambient*light.ambient;
    halfV = normalize(halfVector);
    ndothv = max(dot(N, halfV), 0.0);
    specular+=material.specular*light.specular*pow(ndothv, material.shininess);
  }
return ambient + diffuse + specular;
}


// @brief functions to do the tonemapping

vec3 getExposure(in vec3 color)
{
    vec3 retColor;

    color *= 1.1;

    retColor = pow(color,vec3(1/2.2));


    return retColor;
}


// convert the color range to HDR
vec3 convertToHDR( in vec3 color)
{
    vec3 HDR;

    vec3 overExposed = color *1.2;
    vec3 underExposed = color/1.5;

    HDR = mix(underExposed,overExposed,color);

    return HDR;
}

// Reinhard filter
vec3 Reinhard(in vec3 color)
{
    color = color/(1+color);

    return pow(color,vec3(1/2.2));

}

//Burgess filter
vec3 Burgess (in vec3 color)

{
    vec3 maxColor = max(vec3(0.0),color-0.004);
    vec3 retColor = (maxColor*(6.2*maxColor+0.5))/(maxColor*(6.2*maxColor +1.7)+0.06);


    return pow(retColor,vec3(1/2.2));

}

// color correction
vec3 ColourCorrect (in vec3 color)
{
    color = color + vec3(0.63,-0.12,0.1);
    return color;

}



void main ()
{

   //@brief pixelated shader
   //when pixel at x,y is duplicated into x+dx,y+dy rectangle
    float dx = 15*(1./512.);
    float dy = 20.0*(1./512.);
    vec2 coord = vec2(dx*floor(vertUV.x/dx),
                      dy*floor(vertUV.y/dy));


  vec3 color = pointLight().xyz;

  color =  getExposure(color);
  color = convertToHDR(color);
  color = Burgess(color);

  color = ColourCorrect(color);
  fragColour = vec4 (fragmentNormal,1.0f)*vec4 (color,0)*texture2D(tex, coord);

}

