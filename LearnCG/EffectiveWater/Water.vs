#version 330
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;
uniform float gTime;
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

struct Wave
{
    float L; //波长
    float A; //振幅
    float W; //频率
    float S; //速度
    float P; //相位
    float Q; //控制参数
    vec2 Dir;//方向向量
};

const int MAX_WAVE_NUM = 2;
uniform int gWaveNum;
uniform Wave gWaves[MAX_WAVE_NUM];

mat3 CalcTBN(vec2 Pos, float t)
{
    vec3 T = vec3(1, 0, 0);
    for (int i = 0; i < gWaveNum; ++i) {
        float WA = gWaves[i].W*gWaves[i].A;
        float S0 = sin(gWaves[i].W*dot(gWaves[i].Dir, Pos)+gWaves[i].P*t);
        float C0 = cos(gWaves[i].W*dot(gWaves[i].Dir, Pos)+gWaves[i].P*t);
        T.x -= gWaves[i].Q*gWaves[i].Dir.x*gWaves[i].Dir.x*WA*S0;
        T.z -= gWaves[i].Q*gWaves[i].Dir.y*gWaves[i].Dir.y*WA*S0;
        T.y += gWaves[i].Dir.x*WA*C0;
    }
    
    vec3 B = vec3(0, 0, 1);
    for (int i = 0; i < gWaveNum; ++i) {
        float WA = gWaves[i].W*gWaves[i].A;
        float S0 = sin(gWaves[i].W*dot(gWaves[i].Dir, Pos)+gWaves[i].P*t);
        float C0 = cos(gWaves[i].W*dot(gWaves[i].Dir, Pos)+gWaves[i].P*t);
        B.x -= gWaves[i].Q*gWaves[i].Dir.x*gWaves[i].Dir.y*WA*S0;
        B.z -= gWaves[i].Q*gWaves[i].Dir.y*gWaves[i].Dir.y*WA*S0;
        B.y += gWaves[i].Dir.y*WA*C0;
    }
    
    vec3 N = vec3(0, 1, 0);
    for (int i = 0; i < gWaveNum; ++i) {
        float WA = gWaves[i].W*gWaves[i].A;
        float S0 = sin(gWaves[i].W*dot(gWaves[i].Dir, Pos)+gWaves[i].P*t);
        float C0 = cos(gWaves[i].W*dot(gWaves[i].Dir, Pos)+gWaves[i].P*t);
        N.x -= gWaves[i].Dir.x*WA*C0;
        N.z -= gWaves[i].Dir.y*WA*C0;
        N.y -= gWaves[i].Q*WA*S0;
    }
    return mat3(T, B, N);
    //return mat3(normalize(T), normalize(B), normalize(N));
}

vec3 CalcPos(vec2 Pos, float t)
{
    float x = Pos.x, y = Pos.y, h = 0;
    for (int i = 0; i < gWaveNum; ++i) {
        float W = gWaves[i].W;
        float A = gWaves[i].A;
        float P = gWaves[i].P;
        float Q = gWaves[i].Q;
        vec2 Dir = gWaves[i].Dir;
        x += Q*A*Dir.x*cos(dot(Dir, Pos)*W+P*t);
        y += Q*A*Dir.y*cos(dot(Dir, Pos)*W+P*t);
        h += A*sin(dot(Dir, Pos)*W+P*t);
    }
    return vec3(x, h, y);
}

out mat3 TBN;
out vec3 WorldPos0;
out vec2 TexCoord0;
void main()
{
    vec3 WorldPos = (gWorldMat * vec4(Position, 1)).xyz;
    vec3 Pos = CalcPos(WorldPos.xz, gTime);
    
    TBN = CalcTBN(WorldPos.xz, gTime);
    WorldPos0 = Pos;
    TexCoord0 = TexCoord;
    gl_Position = gProjMat * gViewMat * vec4(Pos, 1);
}
