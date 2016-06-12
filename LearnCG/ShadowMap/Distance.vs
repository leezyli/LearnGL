#version 330
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;
layout(location = 0) in vec3 Position;

out vec3 WorldPos0;
void main()
{
    WorldPos0 = (gWorldMat * vec4(Position, 1)).xyz;
    gl_Position = gProjMat * gViewMat * gWorldMat * vec4(Position, 1);
}