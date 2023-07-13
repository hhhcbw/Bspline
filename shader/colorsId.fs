#version 330 core

//flat in uint fId;
in float fId;

//out uvec4 FragColor;
out vec4 FragColor;

void main()
{
    //uvec4 color = uvec4(fId, 0, 0, 255);
    //vec4 floatColor = vec4(color) / 255.0;
    //FragColor = floatColor;
    FragColor = vec4(fId, 0.0, 0.0, 1.0);
}