#version 330
layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices=18) out;

uniform vec3 gLightPos;
uniform vec3 gLightDir;
uniform mat4 gViewMat;
uniform mat4 gProjMat;

const float EXP = 0.0000;

void EmitQuad(vec3 v0, vec3 v1, vec3 LightDir, mat4 ProjViewMat)
{
    //LightDir = normalize(v0 - gLightPos);
    gl_Position = ProjViewMat * vec4(v0+LightDir*EXP, 1);
    EmitVertex();
    gl_Position = ProjViewMat * vec4(LightDir, 0);
    EmitVertex();
    //LightDir = normalize(v1 - gLightPos);
    gl_Position = ProjViewMat * vec4(v1+LightDir*EXP, 1);
    EmitVertex();
    gl_Position = ProjViewMat * vec4(LightDir, 0);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    mat4 ProjViewMat = gProjMat * gViewMat;
    
    vec3 v0 = gl_in[0].gl_Position.xyz;
    vec3 v1 = gl_in[1].gl_Position.xyz;
    vec3 v2 = gl_in[2].gl_Position.xyz;
    vec3 v3 = gl_in[3].gl_Position.xyz;
    vec3 v4 = gl_in[4].gl_Position.xyz;
    vec3 v5 = gl_in[5].gl_Position.xyz;
    
    vec3 e0 = normalize(v0 - v2);
    vec3 e1 = normalize(v4 - v2);
    vec3 Normal = normalize(cross(e1, e0));
    vec3 LightDir = normalize(gLightDir);
    if ( dot(-LightDir, Normal) > 0 ) {
        e0 = normalize(v1 - v0);
        e1 = normalize(v2 - v0);
        Normal = normalize(cross(e0, e1));
        if ( dot(-LightDir, Normal) <= 0 )
            EmitQuad(v0, v2, LightDir, ProjViewMat);
        
        //LightDir = normalize(v2 - gLightPos);
        e0 = normalize(v4 - v2);
        e1 = normalize(v3 - v2);
        Normal = normalize(cross(e1, e0));
        if ( dot(-LightDir, Normal) <= 0 )
            EmitQuad(v2, v4, LightDir, ProjViewMat);
        
        //LightDir = normalize(v4 - gLightPos);
        e0 = normalize(v0 - v4);
        e1 = normalize(v5 - v4);
        Normal = normalize(cross(e1, e0));
        if ( dot(-LightDir, Normal) <= 0 )
            EmitQuad(v4, v0, LightDir, ProjViewMat);
        
        // top cap
        //LightDir = normalize(v0 - gLightPos);
        gl_Position = ProjViewMat * vec4(v0+LightDir*EXP, 1);
        EmitVertex();
        //LightDir = normalize(v2 - gLightPos);
        gl_Position = ProjViewMat * vec4(v2+LightDir*EXP, 1);
        EmitVertex();
        //LightDir = normalize(v4 - gLightPos);
        gl_Position = ProjViewMat * vec4(v4+LightDir*EXP, 1);
        EmitVertex();
        EndPrimitive();
        
        // back cap
        /*LightDir = normalize(v0 - gLightPos);
        gl_Position = ProjViewMat * vec4(LightDir, 0);
        EmitVertex();
        LightDir = normalize(v4 - gLightPos);
        gl_Position = ProjViewMat * vec4(LightDir, 0);
        EmitVertex();
        LightDir = normalize(v2 - gLightPos);
        gl_Position = ProjViewMat * vec4(LightDir, 0);
        EmitVertex();
        EndPrimitive();*/
    }
}