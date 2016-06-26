#version 330
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;
layout(location = 0) in vec3 Position;

void main()
{
    mat4 gWVP = gProjMat * gViewMat * gWorldMat;
    gl_Position = gWVP * vec4(Position, 1);
}