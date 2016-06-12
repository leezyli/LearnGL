#version 330
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;
uniform mat4 gLightProjMat;
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

out vec4 LightProjPos;
out vec3 WorldPos0;
out vec3 Normal0;
out vec2 TexCoord0;
void main()
{
    WorldPos0 = (gWorldMat * vec4(Position, 1)).xyz;
    Normal0 = normalize((transpose(inverse(gWorldMat))*vec4(Normal,1)).xyz);
    TexCoord0 = TexCoord;
    LightProjPos = gLightProjMat * vec4(WorldPos0, 1);
    
    gl_Position = gProjMat * gViewMat * gWorldMat * vec4(Position, 1);
}