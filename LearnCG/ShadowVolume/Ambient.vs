#version 330
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

out vec2 TexCoord0;
void main()
{
    TexCoord0 = TexCoord;
    gl_Position = gProjMat * gViewMat * gWorldMat * vec4(Position, 1);
}