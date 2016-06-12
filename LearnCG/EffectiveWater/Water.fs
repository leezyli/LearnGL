#version 330
uniform sampler2D gSampler;
uniform sampler2D gNormalMap;

struct LightBase
{
    vec3 LightPos;//光源的位置
    vec4 LightColor;//光的颜色
    vec4 AmbientColor;//环境光的颜色
    float Shiness;
};

struct DirectionalLight
{
    LightBase Light;
    vec3 LightToPos;//平行光照射的参考点
};

vec4 CalcLight_Core(LightBase Light, vec3 LightDir, vec3 WorldPos, vec3 Normal, vec3 CameraPos)
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
            vec4  Specular = vec4(Shiness, Shiness, Shiness, 1);
            SpecularColor = Light.LightColor * Specular;
        }
        DiffuseColor = Light.LightColor * Diffuse;
    }
    return Light.AmbientColor + DiffuseColor + SpecularColor;
}

vec3 CalcNormal(mat3 TBN, vec2 TexCoord)
{
    vec3 Normal = texture(gNormalMap, TexCoord).rgb;
    Normal = 2.0 * Normal - 1.0;
    vec3 NewNormal = TBN*Normal;
    return normalize(NewNormal);
}

uniform vec3 gCameraPos;
uniform DirectionalLight gLight;

in mat3 TBN;
in vec3 WorldPos0;
in vec2 TexCoord0;
out vec4 FragColor;
void main()
{
    //FragColor = vec4(1, 0, 0, 1);
    vec3 Normal = CalcNormal(TBN, TexCoord0);
    vec3 LightDir = normalize(gLight.LightToPos - gLight.Light.LightPos);
    vec4 Color = CalcLight_Core(gLight.Light, LightDir, WorldPos0, Normal, gCameraPos);
   
    FragColor = texture(gSampler, TexCoord0)*Color;
}