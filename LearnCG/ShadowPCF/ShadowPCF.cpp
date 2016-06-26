//
//  ShadowPCF.cpp
//  LearnCG
//
//  Created by leezy on 16/6/23.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "ShadowPCF.h"
#include "UtilCG.h"

Camera ShadowPCF::gCamera;
Pipeline ShadowPCF::gPipeline;
ShadowPCF* ShadowPCF::gThisDemo = NULL;


ShadowPCF::ShadowPCF()
{
    mEnablePCF = false;
    mVAO = 0;
    mDepthProg = INVALID_HANDLE;
    mLightProg = INVALID_HANDLE;
    mPCFProg = INVALID_HANDLE;
}

ShadowPCF::~ShadowPCF()
{
    if ( mVAO )
        glDeleteVertexArrays(1, &mVAO);
    if ( INVALID_HANDLE != mDepthProg )
        glDeleteProgram(mDepthProg);
    if ( INVALID_HANDLE != mLightProg )
        glDeleteProgram(mLightProg);
    if ( INVALID_HANDLE != mPCFProg )
        glDeleteProgram(mPCFProg);
}

bool ShadowPCF::Init()
{
    glfwSetErrorCallback(glfwErrorCallback_GPU);
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    mpWindow = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Percentage Closer Filtering", NULL, NULL);
    if ( !mpWindow )
    {
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(mpWindow);
    glfwSetKeyCallback(mpWindow, OnCallbackKeyEvnt);
    glfwSetFramebufferSizeCallback(mpWindow, OnFramebufferSize);
    
    int width = 0, height = 0;
    glfwGetFramebufferSize(mpWindow, &width, &height);
    
    if ( !LoadDepthShader() )
        return false;
    
    if ( !LoadLightShader() )
        return false;
    
    if ( !LoadLightShader_PCF() )
        return false;
    
    if ( !mDepthTex.Load(width, height) )
        return false;
    
    if ( !mGroundTex.Load(g_AssetsPath+"/Assets/ground.png") )
        return false;
    
    if ( !mMonster.Load(g_AssetsPath+"/Assets/Monster/yanmo.obj") )
        return false;
    
    gCamera.LookAt(glm::vec3(150, 150, 150), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    gPipeline.SetPerspective(45, (GLfloat)width/height, 1, 1500);
    gThisDemo = this;
    return true;
}

void ShadowPCF::OnFramebufferSize(GLFWwindow *pWindow, int width, int height)
{
    gPipeline.SetPerspective(45, (GLfloat)width/height, 1, 1500);
}

void ShadowPCF::OnCallbackKeyEvnt(GLFWwindow *pWindow, int key, int scancode, int action, int mode)
{
    if ( action == GLFW_PRESS && key == GLFW_KEY_ENTER )
    {
        if ( gThisDemo )
            gThisDemo->ChangeRenderMode();
    }
}

bool ShadowPCF::LoadDepthShader()
{
    std::string VShader = g_AssetsPath+"/ShadowPCF/Depth.vs";
    std::string FShader = g_AssetsPath+"/ShadowPCF/Depth.fs";
    mDepthProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mDepthProg != INVALID_HANDLE;
}

bool ShadowPCF::LoadLightShader()
{
    std::string VShader = g_AssetsPath+"/ShadowPCF/Shadow.vs";
    std::string FShader = g_AssetsPath+"/ShadowPCF/Shadow.fs";
    mLightProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mLightProg != INVALID_HANDLE;
}

bool ShadowPCF::LoadLightShader_PCF()
{
    std::string VShader = g_AssetsPath+"/ShadowPCF/Shadow.vs";
    std::string FShader = g_AssetsPath+"/ShadowPCF/ShadowPCF.fs";
    mPCFProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mPCFProg != INVALID_HANDLE;
}

void ShadowPCF::ChangeRenderMode()
{
    mEnablePCF ^= 1;
}

void ShadowPCF::RunLoop()
{
    glm::vec3 Position[4] = {
        glm::vec3(-250, 0, -250),
        glm::vec3(-250, 0, 250),
        glm::vec3(250, 0, -250),
        glm::vec3(250, 0, 250)
    };
    
    glm::vec3 Normal[4] = {
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0)
    };
    
    glm::vec2 TexCoord[4] = {
        glm::vec2(0, 0),
        glm::vec2(0, 1),
        glm::vec2(1, 0),
        glm::vec2(1, 1)
    };
    
    GLushort Indices[4] = {0, 1, 2, 3};
    
    GLuint VBO[4];
    glGenBuffers(4, VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Position), &Position[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normal), &Normal[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoord), &TexCoord[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices[0], GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    mGroundMat = glm::translate(glm::mat4(1), glm::vec3(-100, 0, -100));
    mMonsterMat = glm::scale(glm::mat4(1), glm::vec3(2, 2, 2));
    mMonsterMat = glm::translate(mMonsterMat, glm::vec3(0, 0, 10));
    
    mLight.SetLightPos(glm::vec3(-50, 80, 50));
    mLight.SetCastPos(glm::vec3(0, 0, -50));
    mLight.SetLightColor(glm::vec4(1, 1, 1, 1));
    mLight.SetLightIntensity(1.5);
    mLight.SetAmbientColor(glm::vec4(1, 1, 1, 1));
    mLight.SetAmbientIntensity(1);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    while ( !glfwWindowShouldClose(mpWindow) ) {
        OnRender();
        glfwPollEvents();
        glfwSwapBuffers(mpWindow);
    }
    
    glfwTerminate();
}

void ShadowPCF::OnRender()
{
    glClearColor(0, 0, 0, 0);
    
    if ( !mEnablePCF )
    {
        RenderDepthPass_DirectLight();
        RenderLightPass_DirectLight();
    }
    else
    {
        RenderDepthPass_DirectLight();
        RenderLightPass_DirectLight_PCF();
    }
}

void ShadowPCF::RenderDepthPass_DirectLight()
{
    mDepthTex.BindForDraw();
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mDepthProg);
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2, 0.1);
    
    mLightCame.LookAt(mLight.GetLightPos(), mLight.GetCastPos(), gCamera.GetUp());
    mLightPipe.SetOrth(-250, 250, -250, 250, 1, 500);
    
    {
        GLint gWorldMat = glGetUniformLocation(mDepthProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mDepthProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mDepthProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mMonsterMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(mLightCame.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(mLightPipe.GetProjMat()));
        
        mMonster.Render();
    }
    
    {
        GLint gWorldMat = glGetUniformLocation(mDepthProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mDepthProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mDepthProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mGroundMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(mLightCame.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(mLightPipe.GetProjMat()));
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
    
    glUseProgram(0);
}

void ShadowPCF::RenderLightPass_DirectLight()
{
    mDepthTex.BindForRead(GL_TEXTURE1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mLightProg);
        
    GLint gShadowMap = glGetUniformLocation(mLightProg, "gShadowMap");
    glUniform1i(gShadowMap, mDepthTex.GetTex());
    
    glm::mat4 LightProjMat = mLightPipe.GetProjMat() * mLightCame.GetViewMat();
    GLint gLightProjMat = glGetUniformLocation(mLightProg, "gLightProjMat");
    glUniformMatrix4fv(gLightProjMat, 1, GL_FALSE, glm::value_ptr(LightProjMat));
    
    GLint gCameraPos = glGetUniformLocation(mLightProg, "gCameraPos");
    glUniform3fv(gCameraPos, 1, glm::value_ptr(gCamera.GetEyePos()));
    
    GLint LightColor = glGetUniformLocation(mLightProg, "gDirectLight.Base.LightColor");
    GLint AmbientColor = glGetUniformLocation(mLightProg, "gDirectLight.Base.AmbientColor");
    GLint Shiness = glGetUniformLocation(mLightProg, "gDirectLight.Base.Shiness");
    GLint LightPos = glGetUniformLocation(mLightProg, "gDirectLight.LightPos");
    GLint LightCast = glGetUniformLocation(mLightProg, "gDirectLight.LightCast");
    glUniform4fv(LightColor, 1, glm::value_ptr(mLight.GetLightColor()));
    glUniform4fv(AmbientColor, 1, glm::value_ptr(mLight.GetAmbientColor()));
    glUniform1f(Shiness, 128);
    glUniform3fv(LightPos, 1, glm::value_ptr(mLight.GetLightPos()));
    glUniform3fv(LightCast, 1, glm::value_ptr(mLight.GetCastPos()));
    
    {
        GLint gSampler = glGetUniformLocation(mLightProg, "gSampler");
        glUniform1i(gSampler, 0);
        
        GLint gWorldMat = glGetUniformLocation(mLightProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mLightProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mLightProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mMonsterMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mMonster.Render();
    }
    
    {
        mGroundTex.Bind(GL_TEXTURE0);
        GLint gSampler = glGetUniformLocation(mLightProg, "gSampler");
        glUniform1i(gSampler, mGroundTex.GetTex());
        
        GLint gWorldMat = glGetUniformLocation(mLightProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mLightProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mLightProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mGroundMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
    
    glUseProgram(0);
}

void ShadowPCF::RenderLightPass_DirectLight_PCF()
{
    mDepthTex.BindForRead(GL_TEXTURE1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mPCFProg);
    
    GLint gShadowMap = glGetUniformLocation(mPCFProg, "gShadowMap");
    glUniform1i(gShadowMap, mDepthTex.GetTex());
    
    GLint gMapWidth = glGetUniformLocation(mPCFProg, "gMapWidth");
    GLint gMapHeight = glGetUniformLocation(mPCFProg, "gMapHeight");
    glUniform1i(gMapWidth, mDepthTex.GetWidth());
    glUniform1i(gMapHeight, mDepthTex.GetHeight());
    
    glm::mat4 LightProjMat = mLightPipe.GetProjMat() * mLightCame.GetViewMat();
    GLint gLightProjMat = glGetUniformLocation(mPCFProg, "gLightProjMat");
    glUniformMatrix4fv(gLightProjMat, 1, GL_FALSE, glm::value_ptr(LightProjMat));
    
    GLint gCameraPos = glGetUniformLocation(mPCFProg, "gCameraPos");
    glUniform3fv(gCameraPos, 1, glm::value_ptr(gCamera.GetEyePos()));
    
    GLint LightColor = glGetUniformLocation(mPCFProg, "gDirectLight.Base.LightColor");
    GLint AmbientColor = glGetUniformLocation(mPCFProg, "gDirectLight.Base.AmbientColor");
    GLint Shiness = glGetUniformLocation(mPCFProg, "gDirectLight.Base.Shiness");
    GLint LightPos = glGetUniformLocation(mPCFProg, "gDirectLight.LightPos");
    GLint LightCast = glGetUniformLocation(mPCFProg, "gDirectLight.LightCast");
    glUniform4fv(LightColor, 1, glm::value_ptr(mLight.GetLightColor()));
    glUniform4fv(AmbientColor, 1, glm::value_ptr(mLight.GetAmbientColor()));
    glUniform1f(Shiness, 128);
    glUniform3fv(LightPos, 1, glm::value_ptr(mLight.GetLightPos()));
    glUniform3fv(LightCast, 1, glm::value_ptr(mLight.GetCastPos()));
    
    {
        GLint gSampler = glGetUniformLocation(mPCFProg, "gSampler");
        glUniform1i(gSampler, 0);
        
        GLint gWorldMat = glGetUniformLocation(mPCFProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mPCFProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mPCFProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mMonsterMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mMonster.Render();
    }
    
    {
        mGroundTex.Bind(GL_TEXTURE0);
        GLint gSampler = glGetUniformLocation(mPCFProg, "gSampler");
        glUniform1i(gSampler, mGroundTex.GetTex());
        
        GLint gWorldMat = glGetUniformLocation(mPCFProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mPCFProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mPCFProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mGroundMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
    
    glUseProgram(0);
}
