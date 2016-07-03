#version 330

struct LightBase
{
    vec4 LightColor;
    vec4 AmbientColor;
    float Shiness;
};

vec4 CalcLight_Core(LightBase Light, vec3 LightDir, vec3 WorldPos, vec3 Normal, vec3 CameraPos)
{
    vec4 DiffuseColor = vec4(0);
    vec4 SpecularColor = vec4(0);
    float Diffuse = max(0, dot(-LightDir, Normal));
    if ( Diffuse > 0 ) {
        vec3 EyeVector = normalize(CameraPos - WorldPos);
        vec3 Reflect = normalize(reflect(LightDir, Normal));
        float Factor = max(0, dot(Reflect, EyeVector));
        Factor = pow(Factor, Light.Shiness);
        vec4 Specular = vec4(Factor, Factor, Factor, 1);
        DiffuseColor = Diffuse * Light.LightColor;
        SpecularColor = Specular * Light.LightColor;
    }
    return Light.AmbientColor + DiffuseColor + SpecularColor;
}

struct DirectionLight
{
    vec3 LightPos;
    vec3 LightCast;
    LightBase Base;
};

vec4 CalcDirectionLight(DirectionLight Light, vec3 WorldPos, vec3 Normal, vec3 CameraPos)
{
    Normal = normalize(Normal);
    vec3 LightDir = normalize(Light.LightCast - Light.LightPos);
    return CalcLight_Core(Light.Base, LightDir, WorldPos, Normal, CameraPos);
}

uniform sampler2D gSampler;
uniform vec3 gCameraPos;
uniform DirectionLight gLight;
uniform bool gEnableSSAO;

uniform int gScreenWidth;
uniform int gScreenHeight;
vec2 GetTexCoord()
{
    float x = gl_FragCoord.x / gScreenWidth;
    float y = gl_FragCoord.y / gScreenHeight;
    return vec2(x, y);
}

in vec3 WorldPos0;
in vec3 Normal0;
out vec4 FragColor;
void main()
{
    vec4 Color = CalcDirectionLight(gLight, WorldPos0, Normal0, gCameraPos);
    if ( !gEnableSSAO )
    {
        FragColor = Color;
    }
    else
    {
        float AO = texture(gSampler, GetTexCoord()).r;
        FragColor = Color * AO;
    }
}