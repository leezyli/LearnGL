#version 330
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;

out vec3 WorldPos0;
out vec3 Normal0;
void main()
{
    mat4 gWVP = gProjMat * gViewMat * gWorldMat;
    
    WorldPos0 = (gWorldMat * vec4(Position, 1)).xyz;
    Normal0 = (transpose(inverse(gWorldMat)) * vec4(Normal, -dot(Position, Normal))).xyz;
    Normal0 = normalize(Normal0);
    gl_Position = gWVP * vec4(Position, 1);
}