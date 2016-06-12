#version 330
uniform sampler2D gSampler;
uniform vec4 gAmbient;

in vec2 TexCoord0;
out vec4 FragColor;
void main()
{
    vec4 Color = texture(gSampler, TexCoord0);
    FragColor = gAmbient * Color;
}