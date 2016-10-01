#version 330
uniform sampler2D gSampler;

in vec3 WorldPos0;
in vec3 Normal0;
in vec2 TexCoord0;
out vec4 FragColor;
void main()
{
    FragColor = texture(gSampler, TexCoord0);
}