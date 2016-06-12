#version 330
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;
layout(location = 0) in vec3 Position;

void main()
{
    gl_Position = gWorldMat * vec4(Position, 1);
}