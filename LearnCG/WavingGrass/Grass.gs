#version 330
layout(points) in;
layout(triangle_strip, max_vertices=16) out;

uniform float gAngle;
uniform float gHalfWidth;
uniform float gHalfHeight;
uniform vec3 gUp;
uniform vec3 gRight;
uniform mat4 gWorldMat;
uniform mat4 gViewMat;
uniform mat4 gProjMat;

uniform float gTimeStamp;
uniform float gWindStrength;
uniform vec3 gWindDirection;

mat4 Translate(mat4 m, vec3 v)
{
    mat4 Result = m;
    Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
    return Result;
}

mat4 Rotate(mat4 m, float angle, vec3 v)
{
    float a = angle;
    float c = cos(a);
    float s = sin(a);
    
    vec3 axis = normalize(v);
    vec3 temp = (1 - c)*axis;
    
    mat3 Rotate;
    Rotate[0][0] = c + temp[0] * axis[0];
    Rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
    Rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];
    
    Rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
    Rotate[1][1] = c + temp[1] * axis[1];
    Rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];
    
    Rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
    Rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
    Rotate[2][2] = c + temp[2] * axis[2];
    
    mat4 Result;
    Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
    Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
    Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
    Result[3] = m[3];
    return Result;
}

struct VS_IN
{
    vec3 ObjPos;
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
};

struct VS_OUT
{
    vec4 Position;
    vec3 Normal;
    vec2 TexCoord;
};

struct VS_TEMP
{
    vec3 Position;
    vec3 Normal;
};

VS_IN[4] GenQuad(vec3 PA, vec3 PB, float angle)
{
    vec3 ObjPos = gl_in[0].gl_Position.xyz;
    vec3 P1 = PA - gRight*gHalfWidth;
    vec3 P2 = PB - gRight*gHalfWidth;
    vec3 P3 = PA + gRight*gHalfWidth;
    vec3 P4 = PB + gRight*gHalfWidth;
    mat4 WVP = gProjMat * gViewMat * gWorldMat;
    mat4 T1 = Translate(mat4(1), -ObjPos);
    mat4 T2 = Translate(mat4(1), ObjPos);
    
    // 旋转后的Quads
    mat4 R = Rotate(mat4(1), angle, gUp);
    mat4 M = T2 * R * T1;
    P1 = (M * vec4(P1, 1)).xyz;
    P2 = (M * vec4(P2, 1)).xyz;
    P3 = (M * vec4(P3, 1)).xyz;
    P4 = (M * vec4(P4, 1)).xyz;
    
    VS_IN vs[4];
    vs[0].ObjPos = ObjPos;
    vs[0].Position = P1;
    vs[0].Normal = gUp;
    vs[0].TexCoord = vec2(0, 0);
    
    vs[1].ObjPos = ObjPos;
    vs[1].Position = P2;
    vs[1].Normal = gUp;
    vs[1].TexCoord = vec2(0, 1);
    
    vs[2].ObjPos = ObjPos;
    vs[2].Position = P3;
    vs[2].Normal = gUp;
    vs[2].TexCoord = vec2(1, 0);
    
    vs[3].ObjPos = ObjPos;
    vs[3].Position = P4;
    vs[3].Normal = gUp;
    vs[3].TexCoord = vec2(1, 1);
    return vs;
}

vec3 CalcTranslation(vec3 Position, float TimeStamp, vec3 WindDirection, float WindStrength)
{
    return vec3(0);
}

VS_OUT Animation(VS_IN vs)
{
    VS_OUT Out;
    VS_TEMP Temp;
    if ( vs.TexCoord.y < 0.1 )
    {
        vec3 Trans = CalcTranslation(vs.ObjPos, gTimeStamp, gWindDirection, gWindStrength);
        Temp.Position = vs.Position + Trans;
        Temp.Normal = vs.Normal;
    }
    else
    {
        Temp.Position = vs.Position;
        Temp.Normal = vs.Normal;
    }
    
    mat4 WVP = gProjMat * gViewMat * gWorldMat;
    Out.Position = WVP * vec4(Temp.Position, 1);
    Out.Normal = normalize(Temp.Normal);
    Out.TexCoord = vs.TexCoord;
    return Out;
}

out vec3 Normal;
out vec2 TexCoord;
void MakeQuad(VS_OUT vs[4])
{
    gl_Position = vs[0].Position;
    Normal = gUp;
    TexCoord = vs[0].TexCoord;
    EmitVertex();
    gl_Position = vs[1].Position;
    Normal = gUp;
    TexCoord = vs[1].TexCoord;
    EmitVertex();
    gl_Position = vs[2].Position;
    Normal = gUp;
    TexCoord = vs[2].TexCoord;
    EmitVertex();
    gl_Position = vs[3].Position;
    Normal = gUp;
    TexCoord = vs[3].TexCoord;
    EmitVertex();
    EndPrimitive();
}


void main()
{
    vec3 PA = gl_in[0].gl_Position.xyz + gUp*gHalfHeight;
    vec3 PB = gl_in[0].gl_Position.xyz - gUp*gHalfHeight;
    
    {
        VS_IN In[4] = GenQuad(PA, PB, 0);
        VS_OUT Out[4];
        for (int i = 0; i < 4; ++i) {
            Out[i] = Animation(In[i]);
        }
        MakeQuad(Out);
    }
    {
        VS_IN In[4] = GenQuad(PA, PB, gAngle);
        VS_OUT Out[4];
        for (int i = 0; i < 4; ++i) {
            Out[i] = Animation(In[i]);
        }
        MakeQuad(Out);
    }
    {
        VS_IN In[4] = GenQuad(PA, PB, -gAngle);
        VS_OUT Out[4];
        for (int i = 0; i < 4; ++i) {
            Out[i] = Animation(In[i]);
        }
        MakeQuad(Out);
    }
}