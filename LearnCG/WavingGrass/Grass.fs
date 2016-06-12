#version 330
uniform sampler2D gSampler;

in vec3 Normal;
in vec2 TexCoord;
out vec4 FragColor;
void main()
{
    //FragColor = vec4(1, 0, 0, 1);
    FragColor = texture(gSampler, TexCoord);
}