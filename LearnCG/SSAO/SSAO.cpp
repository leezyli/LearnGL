//
//  SSAO.cpp
//  LearnCG
//
//  Created by leezy on 16/7/2.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "SSAO.h"


Camera SSAO::gCamera;
Pipeline SSAO::gPipeline;
SSAO* SSAO::gThisDemo;

SSAO::SSAO()
{
    mVAO =  0;
    mGeomProg = 0;
    mSSAOProg = 0;
    mLightProg = 0;
    mEnableSSAO = false;
}

SSAO::~SSAO()
{
    if ( mVAO )
        glDeleteVertexArrays(1, &mVAO);
    if ( INVALID_HANDLE != mGeomProg )
        glDeleteProgram(mGeomProg);
    if ( INVALID_HANDLE != mSSAOProg )
        glDeleteProgram(mSSAOProg);
    if ( INVALID_HANDLE != mLightProg )
        glDeleteProgram(mLightProg);
}

bool SSAO::Init()
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
    glfwGetFramebufferSize(mpWindow, &mWidth, &mHeight);
    
    if ( !mQuad.Load(g_AssetsPath+"Assets/quad.obj") )
        return false;
    
    if ( !mModel.Load(g_AssetsPath+"Assets/cyclops.obj") )
        return false;
    
    if ( !mAOTex.Load(mWidth, mHeight) )
        return false;
    
    if ( !mDepthTex.Load(mWidth, mHeight) )
        return false;
    
    if ( !LoadGeomShader() )
        return false;
    
    if ( !LoadSSAOShader() )
        return false;
    
    if ( !LoadLightShader() )
        return false;
    
    gThisDemo = this;
    
    gCamera.LookAt(glm::vec3(0, 80, 140), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    gPipeline.SetPerspective(45, (GLfloat)mWidth/mHeight, 1, 500);
    return true;
}

void SSAO::OnCallbackKeyEvnt(GLFWwindow *pWindow, int key, int scancode, int action, int mode)
{
    if ( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
    {
        if ( gThisDemo )
            gThisDemo->ChangeRender();
    }
}

void SSAO::OnFramebufferSize(GLFWwindow *pWindow, int width, int height)
{
    gPipeline.SetPerspective(45, (GLfloat)width/height, 1, 500);
}

bool SSAO::LoadGeomShader()
{
    std::string VShader = g_AssetsPath+"SSAO/Depth.vs";
    std::string FShader = g_AssetsPath+"SSAO/Depth.fs";
    mGeomProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mGeomProg != INVALID_HANDLE;
}

bool SSAO::LoadSSAOShader()
{
    std::string VShader = g_AssetsPath+"SSAO/SSAO.vs";
    std::string FShader = g_AssetsPath+"SSAO/SSAO.fs";
    mSSAOProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mSSAOProg != INVALID_HANDLE;
}

bool SSAO::LoadLightShader()
{
    std::string VShader = g_AssetsPath+"SSAO/Light.vs";
    std::string FShader = g_AssetsPath+"SSAO/Light.fs";
    mLightProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mLightProg != INVALID_HANDLE;
}

void SSAO::ChangeRender()
{
    mEnableSSAO ^= 1;
}

void SSAO::RunLoop()
{
    srandom(static_cast<long>(time(NULL)+getpid()));
    
    mSampleRad = 1.0;
    for (int i = 0; i < MAX_KERNEL_SIZE; ++i) {
        float scale = (float)i/ MAX_KERNEL_SIZE;
        float x = 2 * random() / (float)RAND_MAX - 1.0;
        float y = 2 * random() / (float)RAND_MAX - 1.0;
        float z = 2 * random() / (float)RAND_MAX - 1.0;
        mSamplePos[i] = glm::vec3(x, y, z);
        mSamplePos[i] *= (0.1 + 0.9 * scale *scale);
    }
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    mLight.SetLightPos(glm::vec3(-30, 150, 120));
    mLight.SetCastPos(glm::vec3(10, 0, -50));
    mLight.SetLightColor(glm::vec4(1, 1, 1, 1));
    mLight.SetLightIntensity(1);
    mLight.SetAmbientColor(glm::vec4(1, 1, 1, 1));
    mLight.SetAmbientIntensity(0.8);
    
    mModelMat = glm::scale(glm::mat4(1), glm::vec3(10, 10, 10));
    mModelMat = glm::rotate(mModelMat, RADIAN(180), glm::vec3(0, 1, 0));
    
    while ( !glfwWindowShouldClose(mpWindow) ) {
        OnRender();
        glfwPollEvents();
        glfwSwapBuffers(mpWindow);
    }
    
    glfwTerminate();
}

void SSAO::OnRender()
{
    RenderGeomPass();
    RenderSSAOPass();
    RenderLightPass();
}

void SSAO::RenderGeomPass()
{
    mDepthTex.BindForDraw();
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mGeomProg);
    
    Camera lightCamera;
    lightCamera.LookAt(mLight.GetLightPos(), mLight.GetCastPos(), gCamera.GetUp());
    
    GLint gWorldMat = glGetUniformLocation(mGeomProg, "gWorldMat");
    GLint gViewMat = glGetUniformLocation(mGeomProg, "gViewMat");
    GLint gProjMat = glGetUniformLocation(mGeomProg, "gProjMat");
    glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mModelMat));
    glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(lightCamera.GetViewMat()));
    glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
    
    mModel.Render();
    
    glUseProgram(0);
}

void SSAO::RenderSSAOPass()
{
    mDepthTex.BindForRead(GL_TEXTURE1);
    mAOTex.BindForDraw();
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mSSAOProg);
    
    GLint gSampler = glGetUniformLocation(mSSAOProg, "gSampler");
    glUniform1i(gSampler, mDepthTex.GetTex());
    
    GLint gSampleRad = glGetUniformLocation(mSSAOProg, "gSampleRad");
    GLint gKernel = glGetUniformLocation(mSSAOProg, "gKernel");
    glUniform1f(gSampleRad, mSampleRad);
    glUniform3fv(gKernel, MAX_KERNEL_SIZE, glm::value_ptr(mSamplePos[0]));
    
    GLint gAspect = glGetUniformLocation(mSSAOProg, "gAspect");
    GLint gHalfFov = glGetUniformLocation(mSSAOProg, "gHalfFov");
    GLint gProjMat = glGetUniformLocation(mSSAOProg, "gProjMat");
    glUniform1f(gAspect, (GLfloat)mWidth/mHeight);
    glUniform1f(gHalfFov, tanf(RADIAN(45)/2));
    glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
    
    glFrontFace(GL_CW);
    mQuad.Render();
    glFrontFace(GL_CCW);
    
    glUseProgram(0);
}

void SSAO::RenderLightPass()
{
    mAOTex.BindForRead(GL_TEXTURE1);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mLightProg);
    
    Camera lightCamera;
    lightCamera.LookAt(mLight.GetLightPos(), mLight.GetCastPos(), gCamera.GetUp());
    
    glm::mat4 lightProjtMat = gPipeline.GetProjMat() * lightCamera.GetViewMat();
    GLint gLightProjMat = glGetUniformLocation(mLightProg, "gLightProjMat");
    glUniformMatrix4fv(gLightProjMat, 1, GL_FALSE, glm::value_ptr(lightProjtMat));
    
    GLint gSampler = glGetUniformLocation(mLightProg, "gSampler");
    glUniform1i(gSampler, mAOTex.GetTex());
    
    GLint gEnableSSAO = glGetUniformLocation(mLightProg, "gEnableSSAO");
    glUniform1i(gEnableSSAO, mEnableSSAO);
    
    GLint gCameraPos = glGetUniformLocation(mLightProg, "gCameraPos");
    glUniform3fv(gCameraPos, 1, glm::value_ptr(gCamera.GetEyePos()));
    
    GLint LightColor = glGetUniformLocation(mLightProg, "gLight.Base.LightColor");
    GLint AmbientColor = glGetUniformLocation(mLightProg, "gLight.Base.AmbientColor");
    GLint Shiness = glGetUniformLocation(mLightProg, "gLight.Base.Shiness");
    GLint LightPos = glGetUniformLocation(mLightProg, "gLight.LightPos");
    GLint LightCast = glGetUniformLocation(mLightProg, "gLight.LightCast");
    glUniform4fv(LightColor, 1, glm::value_ptr(mLight.GetLightColor()));
    glUniform4fv(AmbientColor, 1, glm::value_ptr(mLight.GetAmbientColor()));
    glUniform1f(Shiness, 128);
    glUniform3fv(LightPos, 1, glm::value_ptr(mLight.GetLightPos()));
    glUniform3fv(LightCast, 1, glm::value_ptr(mLight.GetCastPos()));
    
    GLint gWorldMat = glGetUniformLocation(mLightProg, "gWorldMat");
    GLint gViewMat = glGetUniformLocation(mLightProg, "gViewMat");
    GLint gProjMat = glGetUniformLocation(mLightProg, "gProjMat");
    glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mModelMat));
    glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
    glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
    mModel.Render();
    
    glUseProgram(0);
}
