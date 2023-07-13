#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float vId;

out float fId;
//flat out uint fId;
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

void main()
{
	//gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_Position = vec4(aPos, 1.0);
    fId = vId / 255.0;
}