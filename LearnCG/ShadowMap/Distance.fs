#version 330
uniform vec3 gLightPos;
in vec3 WorldPos0;
out float FragColor;
void main()
{
    float Distance = length(WorldPos0 - gLightPos);
    FragColor = Distance;
}