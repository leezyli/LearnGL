#version 330
uniform sampler2D gSampler;
uniform sampler2DShadow gShadowMap;

struct LightBase
{
    vec4 LightColor;
    vec4 AmbientColor;
    float Shiness;
};

struct DirectionalLight
{
    vec3 LightPos;
    vec3 LightCast;
    LightBase Base;
};

vec4 CalcLight_Core(LightBase Light, vec3 LightDir, vec3 WorldPos, vec3 Normal, vec3 CameraPos, float ShadowFactor)
{
    vec4 DiffuseColor = vec4(0);
    vec4 SpecularColor = vec4(0);
    float Diffuse = max(0, dot(-LightDir, Normal));
    if ( Diffuse > 0 ) {
        vec3 ReflectN = normalize(reflect(LightDir, Normal));
        vec3 EyeVectorN = normalize(CameraPos - WorldPos);
        float Factor = max(0, dot(ReflectN, EyeVectorN));
        Factor = pow(Factor, Light.Shiness);
        vec4 Specular = vec4(Factor, Factor, Factor, 1);
        DiffuseColor = Diffuse * Light.LightColor;
        SpecularColor = Specular * Light.LightColor;
    }
    
    return Light.AmbientColor + ShadowFactor * (DiffuseColor + SpecularColor);
}

vec4 CalcDirectionalLight(DirectionalLight Light, vec3 WorldPos, vec3 Normal, vec3 CameraPos, float ShadowFactor)
{
    vec3 LightDir = normalize(Light.LightCast - Light.LightPos);
    Normal = normalize(Normal);
    return CalcLight_Core(Light.Base, LightDir, WorldPos, Normal, CameraPos, ShadowFactor);
}

uniform int gMapWidth;
uniform int gMapHeight;
uniform vec3 gCameraPos;
uniform mat4 gLightProjMat;
uniform DirectionalLight gDirectLight;

float CalcShadowFactor(vec3 WorldPos)
{
    vec4 ProjPos = gLightProjMat * vec4(WorldPos, 1);
    ProjPos.xyz /= ProjPos.w;
    
    float s = ProjPos.x * 0.5 + 0.5;
    float t = ProjPos.y * 0.5 + 0.5;
    float r = ProjPos.z * 0.5 + 0.5;
    
    float offsetX = 1/gMapWidth;
    float offsetY = 1/gMapHeight;
    
    float ShadowFactor = 0.0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec2 Offset = vec2(x*offsetX, y*offsetY);
            vec3 TexCoord = vec3(vec2(s, t)+Offset, r);
            ShadowFactor += texture(gShadowMap, TexCoord);
        }
    }
    return ShadowFactor/9;
}

in vec3 WorldPos0;
in vec3 Normal0;
in vec2 TexCoord0;
out vec4 FragColor;
void main()
{
    float Factor = CalcShadowFactor(WorldPos0);
    vec4 Color = CalcDirectionalLight(gDirectLight, WorldPos0, Normal0, gCameraPos, Factor);
    FragColor = texture(gSampler, TexCoord0) * Color;
}