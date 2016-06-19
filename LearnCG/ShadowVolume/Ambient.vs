#version 330
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;
uniform bool gIsSphere;
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

vec2 GenSphereTex(vec3 Vertex, vec3 Normal)
{
    mat4 ViewModelMat = gViewMat * gWorldMat;
    vec3 EyeVertex = normalize((ViewModelMat * vec4(Vertex, 1)).xyz);
    vec3 EyeNormal = normalize((transpose(inverse(ViewModelMat)) * vec4(Normal, 1)).xyz);
    vec3 ReflectVector = EyeVertex - EyeNormal * 2.0 * dot(EyeVertex, EyeNormal);
    ReflectVector.z += 1.0;
    float m = 1.0 / (2.0 * sqrt(dot(ReflectVector, ReflectVector)));
    return vec2(ReflectVector.x * m + 0.5, ReflectVector.y * m + 0.5);
}

out vec2 TexCoord0;
void main()
{
    if ( !gIsSphere )
        TexCoord0 = TexCoord;
    else
        TexCoord0 = GenSphereTex(Position, Normal);
    
    gl_Position = gProjMat * gViewMat * gWorldMat * vec4(Position, 1);
}