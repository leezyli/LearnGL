#version 330
layout(location = 0) in vec3 Position;

uniform float gAspect;
uniform float gHalfFov;

out vec2 ViewRay;
out vec2 TexCoord;
void main()
{
    ViewRay.x = Position.x * gAspect * gHalfFov;
    ViewRay.y = Position.y * gHalfFov;
    TexCoord = (Position.xy + 1) * 0.5;
    gl_Position = vec4(Position, 1);
}