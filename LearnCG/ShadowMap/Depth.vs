#version 330
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;
layout(location = 0) in vec3 Position;

out vec4 WorldPos0;
void main()
{
    WorldPos0 = gProjMat * gViewMat * gWorldMat * vec4(Position, 1);
    WorldPos0.xyz /= WorldPos0.w;
    gl_Position = gProjMat * gViewMat * gWorldMat * vec4(Position, 1);
}