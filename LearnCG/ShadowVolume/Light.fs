#version 330
uniform sampler2D gSampler;

struct LightBase
{
    vec4 LightColor;
    float Shiness;
};

struct DirectionLight
{
    vec3 LightPos;
    vec3 LightCastPos;
    LightBase Base;
};

vec4 CalcLight_Core(LightBase Light, vec3 LightDir, vec3 WorldPos, vec3 Normal, vec3 CameraPos)
{
    vec4 DiffuseColor = vec4(0);
    vec4 SpecularColor = vec4(0);
    float Diffuse = max(0, dot(-LightDir, Normal));
    if ( Diffuse > 0 ) {
        vec3 Reflect = normalize(reflect(LightDir, Normal));
        float Factor = max(0, dot(Reflect, normalize(CameraPos - WorldPos)));
        Factor = pow(Factor, Light.Shiness);
        vec4 Specular = vec4(Factor, Factor, Factor, 1);
        DiffuseColor = Light.LightColor * Diffuse;
        SpecularColor = Light.LightColor * Specular;
    }
    return DiffuseColor + SpecularColor;
}

vec4 CalcDirectionLight(DirectionLight Light, vec3 WorldPos, vec3 Normal, vec3 CameraPos)
{
    vec3 LightDir = normalize(Light.LightCastPos - Light.LightPos);
    return CalcLight_Core(Light.Base, LightDir, WorldPos, Normal, CameraPos);
}

uniform DirectionLight gDirectLight;
uniform vec3 gCameraPos;

in vec3 WorldPos0;
in vec3 Normal0;
in vec2 TexCoord0;
out vec4 FragColor;
void main()
{
    vec4 Color = CalcDirectionLight(gDirectLight, WorldPos0, Normal0, gCameraPos);
    FragColor = texture(gSampler, TexCoord0) * Color;
    //FragColor = vec4(1, 0, 0, 1);
}
