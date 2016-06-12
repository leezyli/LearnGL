//
//  EffectWaterDemo.cpp
//  LearnCG
//
//  Created by leezy on 16/4/26.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "EffectWater.h"
#include <vector>


Camera EffectWaterDemo::gCamera;
Pipeline EffectWaterDemo::gPipeline;

EffectWaterDemo::EffectWaterDemo()
{
    mVAO = 0;
    mVBO = 0;
    mTBO = 0;
    mIBO = 0;
    mProg = 0;
}

EffectWaterDemo::~EffectWaterDemo()
{
   if ( mProg )
       glDeleteProgram(mProg);
    if ( mVAO )
        glDeleteVertexArrays(1, &mVAO);
    if ( mVBO )
        glDeleteBuffers(1, &mVBO);
    if ( mTBO )
        glDeleteBuffers(1, &mTBO);
    if ( mIBO )
        glDeleteBuffers(1, &mIBO);
}

bool EffectWaterDemo::Init()
{
    glfwSetErrorCallback(glfwErrorCallback_GPU);
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    mpWindow = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "EffectWaterDemo for LearnCG", nullptr, nullptr);
    if ( !mpWindow )
    {
        GPU_Log("EffectWaterDemo::Init failed!%d", 1);
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(mpWindow);
    glfwSetWindowSizeCallback(mpWindow, OnWindowSize);
    glfwSetFramebufferSizeCallback(mpWindow, OnFramebufferSize);
    
    if ( !LoadShader() )
        return false;
    
    if ( !mTexture0.Load(g_AssetsPath+"Assets/water-texture.tga") )
        return false;
    
    if ( !mTexture1.Load(g_AssetsPath+"Assets/water-texture-normal.tga") )
        return false;
    
    int width, height;
    glfwGetFramebufferSize(mpWindow, &width, &height);
    glViewport(0, 0, width, height);
    
    gCamera.LookAt(glm::vec3(160, 160, 160), glm::vec3(0), glm::vec3(0,1,0));
    gPipeline.SetPerspective(45, (GLfloat)width/height, 1, 500);
    return true;
}

bool EffectWaterDemo::LoadShader()
{
    std::string VShader = g_AssetsPath+"EffectiveWater/Water.vs";
    std::string FShader = g_AssetsPath+"EffectiveWater/Water.fs";
    mProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mProg != INVALID_HANDLE;
}

void EffectWaterDemo::OnWindowSize(GLFWwindow *pWindow, int width, int height)
{

}

void EffectWaterDemo::OnFramebufferSize(GLFWwindow *pWindow, int width, int height)
{
    glViewport(0, 0, width, height);
    gPipeline.SetPerspective(45, (GLfloat)width/height, 1, 500);
}

void EffectWaterDemo::RunLoop()
{
    /*glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mIBO);
    
    PAIR_V3_V2_F Pos[4] = {
        {glm::vec3(-0.5, 0.5, 0), glm::vec2(0, 1)},
        {glm::vec3(-0.5, -0.5, 0), glm::vec2(0, 0)},
        {glm::vec3(0.5, 0.5, 0), glm::vec2(1, 1)},
        {glm::vec3(0.5, -0.5, 0), glm::vec2(1, 0)}
    };
    
    GLushort Indices[4] = {0, 1, 2, 3};
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Pos), &Pos, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices, GL_STATIC_DRAW);
    
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PAIR_V3_V2_F), 0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PAIR_V3_V2_F), (GLvoid*)sizeof(glm::vec3));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
    
    IntWave();
    GenMesh();
    
    glfwSetTime(0.2); // in seconds
    
    while ( !glfwWindowShouldClose(mpWindow) )
    {
        OnRender();
        glfwSwapBuffers(mpWindow);
        glfwPollEvents();
    }
    
    glfwTerminate();
}

void EffectWaterDemo::IntWave()
{
    mWaves[0].L = 3;
    mWaves[0].A = 3;
    mWaves[0].S = 2;
    mWaves[0].Dir = (glm::vec2(1, 1));
    
    mWaves[1].L = 3;
    mWaves[1].A = 4;
    mWaves[1].S = 3;
    mWaves[1].Dir = (glm::vec2(1, -1));
    
    for (int i = 0; i < MAX_WAVE_NUM; ++i) {
        mWaves[i].W = 2 / mWaves[i].L;
        mWaves[i].P = (2 * mWaves[i].S) / mWaves[i].L;
        mWaves[i].Q = 1 / (mWaves[i].W * mWaves[i].A * MAX_WAVE_NUM);
    }
}

void EffectWaterDemo::GenMesh()
{
    n = 20, m = 20;
    
    std::vector<glm::vec3> Pos(n*m);
    int step = 10;
    int offsetZ = -5;
    for (int i = 0; i < m; ++i) {
        int offsetX = 0;
        for (int j = 0; j < n; ++j) {
            float x = step * offsetX;
            float z = step * offsetZ;
            Pos[i*n+j] = glm::vec3(x, 0, z);
            ++offsetX;
        }
        ++offsetZ;
    }
    
    std::vector<glm::vec2> TexCoord(n*m);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            float u = j/(float)(n-1);
            float v = 1 - i/(float)(m-1);
            TexCoord[i*n+j] = glm::vec2(u, v);
        }
    }
    
    std::vector<GLushort> Indices((m-1)*2*n);
    for (int i = 0; i < m-1; ++i) {
        int x = i, y = 0, dir = 0;
        for (int j = 0; j < 2*n; ++j) {
            Indices[i*2*n+j] = x * n + y;
            if ( !dir ) ++x;
            else ++y, x = i;
            dir = !dir;
        }
    }
    
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mTBO);
    glGenBuffers(1, &mIBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*Pos.size(), &Pos[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, mTBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*TexCoord.size(), &TexCoord[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*Indices.size(), &Indices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mTBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EffectWaterDemo::OnRender()
{
    //glClearColor(1, 1, 1, 1);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mProg);

    GLint gWaveNum = glGetUniformLocation(mProg, "gWaveNum");
    glUniform1i(gWaveNum, MAX_WAVE_NUM);
    
    for (int i = 0; i < MAX_WAVE_NUM; ++i) {
        GLint L = GetObjectUniformLocation(mProg, "gWaves[%d].L", i);
        GLint A = GetObjectUniformLocation(mProg, "gWaves[%d].A", i);
        GLint S = GetObjectUniformLocation(mProg, "gWaves[%d].S", i);
        GLint W = GetObjectUniformLocation(mProg, "gWaves[%d].W", i);
        GLint P = GetObjectUniformLocation(mProg, "gWaves[%d].P", i);
        GLint Q = GetObjectUniformLocation(mProg, "gWaves[%d].Q", i);
        GLint Dir = GetObjectUniformLocation(mProg, "gWaves[%d].Dir", i);
        glUniform1f(L, mWaves[i].L);
        glUniform1f(A, mWaves[i].A);
        glUniform1f(S, mWaves[i].S);
        glUniform1f(W, mWaves[i].W);
        glUniform1f(P, mWaves[i].P);
        glUniform1f(Q, mWaves[i].Q);
        glUniform2fv(Dir, 1, glm::value_ptr(mWaves[i].Dir));
    }
   
    GLint LightPos = glGetUniformLocation(mProg, "gLight.Light.LightPos");
    GLint LightColor = glGetUniformLocation(mProg, "gLight.Light.LightColor");
    GLint AmbientColor = glGetUniformLocation(mProg, "gLight.Light.AmbientColor");
    GLint Shiness = glGetUniformLocation(mProg, "gLight.Light.Shiness");
    GLint LightToPos = glGetUniformLocation(mProg, "gLight.LightToPos");
    glUniform3fv(LightPos, 1, glm::value_ptr(glm::vec3(150, 150, -150)));
    glUniform4fv(LightColor, 1, glm::value_ptr(glm::vec4(1, 1, 1, 1)));
    glUniform4fv(AmbientColor, 1, glm::value_ptr(glm::vec4(1, 1, 1, 1)));
    glUniform1f(Shiness, 64);
    glUniform3fv(LightToPos, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
    
    GLint gTime = glGetUniformLocation(mProg, "gTime");
    double fElapsedTime = glfwGetTime();
    glUniform1f(gTime, fElapsedTime);
  
    mTexture0.Bind(GL_TEXTURE0);
    GLint gSampler = glGetUniformLocation(mProg, "gSampler");
    glUniform1i(gSampler, mTexture0.GetTex());
    
    mTexture1.Bind(GL_TEXTURE1);
    GLint gNormalMap = glGetUniformLocation(mProg, "gNormalMap");
    glUniform1i(gNormalMap, mTexture1.GetTex());
    
    GLint gCameraPos = glGetUniformLocation(mProg, "gCameraPos");
    glUniform3fv(gCameraPos, 1, glm::value_ptr(gCamera.GetEyePos()));
    
    GLint gWorldMat = glGetUniformLocation(mProg, "gWorldMat");
    GLint gViewMat = glGetUniformLocation(mProg, "gViewMat");
    GLint gProjMat = glGetUniformLocation(mProg, "gProjMat");
    
    glm::mat4 worldMat = glm::translate(glm::mat4(1), glm::vec3(-100, 0, -50));
    glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(worldMat));
    glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
    glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
    
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLE_STRIP, (m-1)*2*n, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
    
    glUseProgram(0);
}

