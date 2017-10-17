#version 330 core

#define COLOR_SET 0
#define TEXTURE_SET 1
// this is a pointer to the current 2D texture object
uniform sampler2D tex;
uniform vec4 color;
uniform int colorSet;

in vec2 vertUV;
layout (location=0) out vec4 outColour;

void main ()
{

    if (colorSet == TEXTURE_SET)
    {
        outColour = texture(tex,vertUV);
    }
    else
    {
        outColour = vec4(1,1,1,1);
    }
    outColour *= color;
}
