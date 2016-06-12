#version 330
uniform sampler2D gSampler;
uniform sampler2D gShadowMap;
uniform samplerCube gDistanceMap;

struct LightBase
{
    vec3 LightPos;
    vec4 LightColor;
    vec4 AmbientColor;
    float Shiness;
};

struct PointLight
{
    float Exp;
    float Linear;
    float Constant;
    LightBase Base;
};

struct SpotLight
{
    float CutOff;
    vec3 LightCastPos;
    PointLight Light;
};

struct DirectionalLight
{
    vec3 LightToPos;
    LightBase Base;
};

vec4 CalcLight_Core(LightBase Light, vec3 LightDir, vec3 WorldPos, vec3 Normal, vec3 CameraPos, float ShadowFactor)
{
    vec4 DiffuseColor = vec4(0);
    vec4 SpecularColor = vec4(0);
    float Diffuse = max(0, dot(-LightDir, Normal));
    if ( Diffuse > 0 ) {
        vec3 Reflect = normalize(reflect(LightDir, Normal));
        vec3 EyeVector = normalize(CameraPos - WorldPos);
        float Factor = max(0, dot(Reflect, EyeVector));
        if ( Factor > 0 ) {
            float Shiness = pow(Factor, Light.Shiness);
            vec4 Specular = vec4(Shiness, Shiness, Shiness, 1);
            SpecularColor = Light.LightColor * Specular;
        }
        DiffuseColor = Light.LightColor * Diffuse;
    }
    return Light.AmbientColor + (DiffuseColor + SpecularColor) * ShadowFactor;
}

vec4 CalcDirectionalLight(DirectionalLight Light, vec3 WorldPos, vec3 Normal, vec3 CameraPos, float ShadowFactor)
{
    vec3 LightDir = normalize(Light.LightToPos - Light.Base.LightPos);
    return CalcLight_Core(Light.Base, LightDir, WorldPos, Normal, CameraPos, ShadowFactor);
}

vec4 CalcPointLight(PointLight Light, vec3 WorldPos, vec3 Normal, vec3 CameraPos, float ShadowFactor)
{
    vec3 LightDir = WorldPos - Light.Base.LightPos;
    float Distance = length(LightDir);
    float Exp = Light.Exp;
    float Linear = Light.Linear;
    float Constant = Light.Constant;
    float Attan = Constant + Linear*Distance + Exp*Distance*Distance;
    LightDir = normalize(LightDir);
    vec4 Color = CalcLight_Core(Light.Base, LightDir, WorldPos, Normal, CameraPos, ShadowFactor);
    return Color / Attan;
}

vec4 CalcSpotLight(SpotLight Light, vec3 WorldPos, vec3 Normal, vec3 CameraPos, float ShadowFactor)
{
    vec3 LightDir = normalize(WorldPos - Light.Light.Base.LightPos);
    vec3 LightMainDir = normalize(Light.LightCastPos - Light.Light.Base.LightPos);
    float Factor = dot(LightDir, LightMainDir);
    if ( Factor > Light.CutOff ) {
        vec4 Color = CalcPointLight(Light.Light, WorldPos, Normal, CameraPos, ShadowFactor);
        Factor = pow(Factor, 10);
        return Color * Factor;
    }
    return vec4(0);
}

float CalcShadowFactor(vec4 LightProjPos)
{
    vec3 ProjCoords = LightProjPos.xyz / LightProjPos.w;
    float s = ProjCoords.x * 0.5 + 0.5;
    float t = ProjCoords.y * 0.5 + 0.5;
    float Depth = ProjCoords.z * 0.5 + 0.5;
    float Sample = texture(gShadowMap, vec2(s,t)).r;
    if ( Depth < Sample ) return 1;
    return 0;
}

float CalcShadowFactor_Distance(vec3 LightPos, vec3 WorldPos)
{
    vec3 LightDir = WorldPos - LightPos;
    float Distance = length(LightDir);
    float Sample = texture(gDistanceMap, LightDir).r;
    if ( Distance < Sample + 0.05  ) return 1;
    return 0;
}

uniform vec3 gCameraPos;
uniform SpotLight gSpotLight;
uniform PointLight gPointLight;
uniform DirectionalLight gDirectLight;

in vec4 LightProjPos;
in vec3 WorldPos0;
in vec3 Normal0;
in vec2 TexCoord0;
out vec4 FragColor;
void main()
{
    //float ShadowFactor = CalcShadowFactor(LightProjPos);
    //vec4 Color = CalcSpotLight(gSpotLight, WorldPos0, Normal0, gCameraPos, ShadowFactor);
    //vec4 Color = CalcDirectionalLight(gDirectLight, WorldPos0, Normal0, gCameraPos, ShadowFactor);
    
    float ShadowFactor = CalcShadowFactor_Distance(gPointLight.Base.LightPos, WorldPos0);
    vec4 Color = CalcPointLight(gPointLight, WorldPos0, Normal0, gCameraPos, ShadowFactor);
    
    FragColor = texture(gSampler, TexCoord0) * Color;
    
    //FragColor = vec4(ShadowFactor, 0,0,1);
}