#version 330 core

// this is a pointer to the current 2D texture object
uniform sampler2D tex;
// the vertex UV
in vec2 vertUV;

//smooth in float time;

out vec4 outColour;


//@brief declare the tonemapping functions


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




vec3 convertToHDR( in vec3 color)
{
    vec3 HDR;

    vec3 overExposed = color *1.2;
    vec3 underExposed = color/1.5;

    HDR = mix(underExposed,overExposed,color);

    return HDR;
}


void main ()
{



    //vertUV = vec2(time*0.1,0.0);
    vec3 color = texture2D(tex,vertUV).xyz;


    color = Reinhard(color);
    color = Burgess(color);
    color = convertToHDR(color);

    //apply the pixelation filter
    float dx = 2.0*(1./512.);
    float dy = 5.0*(1./512.);
    vec2 coord = vec2(dx*floor(vertUV.x/dx),
                      dy*floor(vertUV.y/dy));
    outColour = texture2D(tex, coord);


}





















