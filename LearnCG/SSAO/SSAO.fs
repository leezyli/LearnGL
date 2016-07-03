#version 330
uniform sampler2D gSampler;
uniform mat4 gProjMat;

const int MAX_KERNEL_SIZE = 64;
uniform float gSampleRad;
uniform vec3 gKernel[MAX_KERNEL_SIZE];

float CalcViewZ(vec2 TexCoord)
{
    float Depth = texture(gSampler, TexCoord).r;
    float ViewZ = gProjMat[3][2] / (2 * Depth - 1 - gProjMat[2][2]);
    return ViewZ;
}

in vec2 ViewRay;
in vec2 TexCoord;

out vec4 FragColor;
void main()
{
    float ViewZ = CalcViewZ(TexCoord);
    float ViewX = ViewRay.x * ViewZ;
    float ViewY = ViewRay.y * ViewZ;
    vec3 Pos = vec3(ViewX, ViewY, ViewZ);
  
    float AO = 0.0;
    for (int i = 0; i < MAX_KERNEL_SIZE; ++i)
    {
        vec3 SamplePos = Pos + gKernel[i];
        vec4 Offset = vec4(SamplePos, 1.0);
        Offset = gProjMat * Offset;
        Offset.xy /= Offset.w;
        Offset.xy = (Offset.xy + 1) * 0.5;
        
        float SampleDepth = CalcViewZ(Offset.xy);
        if ( abs(Pos.z - SampleDepth) < gSampleRad )
            AO += step(SampleDepth, SamplePos.z);
    }
    
    AO = 1.0 - AO/128;
    FragColor = vec4(pow(AO, 1.0));
}