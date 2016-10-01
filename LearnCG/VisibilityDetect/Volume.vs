#version 330
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

out vec3 WorldPos0;
out vec3 Normal0;
out vec2 TexCoord0;
void main()
{
    mat4 gWVP = gProjMat * gViewMat * gWorldMat;
    
    WorldPos0 = (gWorldMat * vec4(Position, 1)).xyz;
    Normal0 = normalize((transpose(inverse(gWorldMat)) * vec4(Normal, -dot(Position, Normal))).xyz);
    TexCoord0 = TexCoord;
    
    gl_Position = gWVP * vec4(Position, 1);
}