//
//  ShadowVolume.cpp
//  LearnCG
//
//  Created by leezy on 16/5/29.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "ShadowVolume.h"

Camera ShadowVolume::gCamera;
Pipeline ShadowVolume::gPipeline;

ShadowVolume::ShadowVolume()
{
    mGemProg = 0;
    mSteProg = 0;
    mLightProg = 0;
    mAmbientProg = 0;
    mVAO = 0;
    mVBO = 0;
    mIBO = 0;
}

ShadowVolume::~ShadowVolume()
{
    if ( mGemProg )
        glDeleteProgram(mGemProg);
    if ( mSteProg )
        glDeleteProgram(mSteProg);
    if ( mLightProg )
        glDeleteProgram(mLightProg);
    if ( mAmbientProg )
        glDeleteProgram(mAmbientProg);
    if ( mVAO )
        glDeleteVertexArrays(1, &mVAO);
    if ( mVBO )
        glDeleteBuffers(1, &mVBO);
    if ( mIBO )
        glDeleteBuffers(1, &mIBO);
}

bool ShadowVolume::Init()
{
    glfwSetErrorCallback(glfwErrorCallback_GPU);
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    mpWindow = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "ShadowVolume for LearnCG", NULL, NULL);
    if ( !mpWindow )
    {
        GPU_Log("ShadowVolume::Init failed!%d", 1);
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(mpWindow);
    glfwSetFramebufferSizeCallback(mpWindow, OnFramebufferSize);
    
    int width, height;
    glfwGetFramebufferSize(mpWindow, &width, &height);
    
    if ( !mModel.Load(g_AssetsPath+"Assets/Bucket/MedievalBarrel_OBJ.obj", EModelFlag_Normal|EModelFlag_TriangleAdjacency))
        return false;
    
    if ( !mTexture.Load(g_AssetsPath+"Assets/ground.png") )
        return false;
    
    if ( !mTexture2.Load(g_AssetsPath+"Assets/diffuse.jpg"))
        return false;
    
    if ( !mDepthTex.Load(width, height) )
        return false;
    
    if ( !LoadDepthShader() )
        return false;
    if ( !LoadStencilShader() )
        return false;
    if ( !LoadLightShader() )
        return false;
    
    if ( !LoadAmbientShader() )
        return false;
    
    glViewport(0, 0, width, height);
    gCamera.LookAt(glm::vec3(50, 80, 120), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    gPipeline.SetPerspective(90, (GLfloat)width/height, 1, 500);
    return true;
}

bool ShadowVolume::LoadDepthShader()
{
    std::string VShader = g_AssetsPath+"ShadowVolume/Depth.vs";
    std::string FShader = g_AssetsPath+"ShadowVolume/Depth.fs";
    mGemProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mGemProg != INVALID_HANDLE;
}

bool ShadowVolume::LoadStencilShader()
{
    std::string VShader = g_AssetsPath+"ShadowVolume/Stencil.vs";
    std::string FShader = g_AssetsPath+"ShadowVolume/Stencil.fs";
    std::string GShader = g_AssetsPath+"ShadowVolume/Stencil.gs";
    mSteProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str(), GShader.c_str());
    return mSteProg != INVALID_HANDLE;
}

bool ShadowVolume::LoadLightShader()
{
    std::string VShader = g_AssetsPath+"ShadowVolume/Light.vs";
    std::string FShader = g_AssetsPath+"ShadowVolume/Light.fs";
    mLightProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mLightProg != INVALID_HANDLE;
}

bool ShadowVolume::LoadAmbientShader()
{
    std::string VShader = g_AssetsPath+"ShadowVolume/Ambient.vs";
    std::string FShader = g_AssetsPath+"ShadowVolume/Ambient.fs";
    mAmbientProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mAmbientProg != INVALID_HANDLE;
}

void ShadowVolume::OnFramebufferSize(GLFWwindow *pWindow, int width, int height)
{
    //glViewport(0, 0, width, height);
    //gPipeline.SetPerspective(45, (GLfloat)width/height, 1, 500);
}

void ShadowVolume::RunLoop()
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    std::vector<Vertex> Vertices(4);
    Vertices[0].Position = glm::vec3(-200, 0, -200);
    Vertices[0].Normal = glm::vec3(0, 1, 0);
    Vertices[0].TexCoord = glm::vec2(0, 0);
    Vertices[1].Position = glm::vec3(-200, 0, 200);
    Vertices[1].Normal = glm::vec3(0, 1, 0);
    Vertices[1].TexCoord = glm::vec2(0, 1);
    Vertices[2].Position = glm::vec3(200, 0, -200);
    Vertices[2].Normal = glm::vec3(0, 1, 0);
    Vertices[2].TexCoord = glm::vec2(1, 0);
    Vertices[3].Position = glm::vec3(200, 0, 200);
    Vertices[3].Normal = glm::vec3(0, 1, 0);
    Vertices[3].TexCoord = glm::vec2(1, 1);
    
    GLushort Indices[] = {0, 1, 2, 3};
    
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mIBO);
    glGenVertexArrays(1, &mVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*Vertices.size(), &Vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(glm::vec3)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    mWorldMat = glm::scale(glm::mat4(1), glm::vec3(10, 10, 10));
    mWorldMat = glm::translate(mWorldMat, glm::vec3(5, 0, 2));
    
    // 平行光
    mDirectLight.SetLightColor(glm::vec4(1, 1, 1, 1));
    mDirectLight.SetLightIntensity(2);
    mDirectLight.SetAmbientColor(glm::vec4(1, 1, 1, 1));
    mDirectLight.SetAmbientIntensity(0.8);
    mDirectLight.SetLightPos(glm::vec3(-50, 100, 100));
    mDirectLight.SetCastPos(glm::vec3(100, 0, -100));
    
    while ( !glfwWindowShouldClose(mpWindow)) {
        OnRender();
        glfwSwapBuffers(mpWindow);
        glfwPollEvents();
    }
    
    glfwTerminate();
}

void ShadowVolume::OnRender()
{
    glDepthMask(GL_TRUE);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    RenderForGeom();
    glEnable(GL_STENCIL_TEST);
    RenderForStencil();
    RenderForLight();
    glDisable(GL_STENCIL_TEST);
    RenderForAmbient();
}

void ShadowVolume::RenderForGeom()
{
    glDrawBuffer(GL_NONE);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
   
    glUseProgram(mGemProg);
    
    {
        GLint gWolrdMat = glGetUniformLocation(mGemProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mGemProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mGemProg, "gProjMat");
        glUniformMatrix4fv(gWolrdMat, 1, GL_FALSE, glm::value_ptr(mWorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mModel.Render();
    }
    
    {
        glm::mat4 WorldMat = glm::scale(glm::mat4(1), glm::vec3(20, 20, 20));
        WorldMat = glm::translate(WorldMat, glm::vec3(-2, 1, 0));
        
        GLint gWorldMat = glGetUniformLocation(mGemProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mGemProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mGemProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(WorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mCube.Render();
    }
    
    {
        GLint gWorldMat = glGetUniformLocation(mGemProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mGemProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mGemProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);
        glBindVertexArray(0);
    }
    
    glUseProgram(0);
}

void ShadowVolume::RenderForStencil()
{
    //glEnable(GL_CULL_FACE);
    //glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.01, 0);
    
    glStencilFunc(GL_ALWAYS, 0x0, 0xFF);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
    
    glUseProgram(mSteProg);
    
    GLint gLightPos = glGetUniformLocation(mSteProg, "gLightPos");
    GLint gLightDir = glGetUniformLocation(mSteProg, "gLightDir");
    glUniform3fv(gLightPos, 1, glm::value_ptr(mDirectLight.GetLightPos()));
    glUniform3fv(gLightDir, 1, glm::value_ptr(mDirectLight.GetCastPos()-mDirectLight.GetLightPos()));
    
    {
        GLint gWorldMat = glGetUniformLocation(mSteProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mSteProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mSteProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mWorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mModel.Render(EModelRender_TriangleAdjacency);
    }
    
    {
        glm::mat4 WorldMat = glm::scale(glm::mat4(1), glm::vec3(20, 20, 20));
        WorldMat = glm::translate(WorldMat, glm::vec3(-2, 1, 0));
        
        GLint gWorldMat = glGetUniformLocation(mSteProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mSteProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mSteProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(WorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mCube.Render(ECUBE_RenderTriangleAdjacency);
    }
    
    glUseProgram(0);
    glDisable(GL_DEPTH_CLAMP);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void ShadowVolume::RenderForLight()
{
    glDrawBuffer(GL_BACK);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    glUseProgram(mLightProg);
    
    GLint gLightColor = glGetUniformLocation(mLightProg, "gDirectLight.Base.LightColor");
    GLint gShiness = glGetUniformLocation(mLightProg, "gDirectLight.Base.Shiness");
    GLint gLightPos = glGetUniformLocation(mLightProg, "gDirectLight.LightPos");
    GLint gLightCastPos = glGetUniformLocation(mLightProg, "gDirectLight.Base.LightCastPos");
    glUniform4fv(gLightColor, 1, glm::value_ptr(mDirectLight.GetLightColor()));
    glUniform1f(gShiness, 128);
    glUniform3fv(gLightPos, 1, glm::value_ptr(mDirectLight.GetLightPos()));
    glUniform3fv(gLightCastPos, 1, glm::value_ptr(mDirectLight.GetCastPos()));
    
    GLint gCameraPos = glGetUniformLocation(mLightProg, "gCameraPos");
    glUniform3fv(gCameraPos, 1, glm::value_ptr(gCamera.GetEyePos()));
    
    {
        mTexture.Bind(GL_TEXTURE0);
        GLint gSampler = glGetUniformLocation(mLightProg, "gSampler");
        glUniform1i(gSampler, mTexture.GetTex());
        
        GLint gWorldMat = glGetUniformLocation(mLightProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mLightProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mLightProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);
        glBindVertexArray(0);
    }
    
    {
        GLint gSampler = glGetUniformLocation(mLightProg, "gSampler");
        glUniform1i(gSampler, 0);
        
        GLint gWolrdMat = glGetUniformLocation(mLightProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mLightProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mLightProg, "gProjMat");
        glUniformMatrix4fv(gWolrdMat, 1, GL_FALSE, glm::value_ptr(mWorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mModel.Render();
    }
    
    {
        mTexture2.Bind(GL_TEXTURE0);
        GLint gSampler = glGetUniformLocation(mLightProg, "gSampler");
        glUniform1i(gSampler, mTexture.GetTex());
        
        glm::mat4 WorldMat = glm::scale(glm::mat4(1), glm::vec3(20, 20, 20));
        WorldMat = glm::translate(WorldMat, glm::vec3(-2, 1, 0));
        
        GLint gWorldMat = glGetUniformLocation(mLightProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mLightProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mLightProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(WorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mCube.Render();
    }
    
    glUseProgram(0);
}

void ShadowVolume::RenderForAmbient()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(mAmbientProg);
    
    GLint gAmbient = glGetUniformLocation(mAmbientProg, "gAmbient");
    glUniform4fv(gAmbient, 1, glm::value_ptr(mDirectLight.GetAmbientColor()));
    
    {
        mTexture.Bind(GL_TEXTURE0);
        GLint gSampler = glGetUniformLocation(mAmbientProg, "gSampler");
        glUniform1i(gSampler, mTexture.GetTex());
        
        GLint gWorldMat = glGetUniformLocation(mAmbientProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mAmbientProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mAmbientProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);
        glBindVertexArray(0);
    }
    
    {
        GLint gSampler = glGetUniformLocation(mAmbientProg, "gSampler");
        glUniform1i(gSampler, 0);
        
        GLint gWolrdMat = glGetUniformLocation(mAmbientProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mAmbientProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mAmbientProg, "gProjMat");
        glUniformMatrix4fv(gWolrdMat, 1, GL_FALSE, glm::value_ptr(mWorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mModel.Render();
    }
    
    {
        mTexture2.Bind(GL_TEXTURE0);
        GLint gSampler = glGetUniformLocation(mAmbientProg, "gSampler");
        glUniform1i(gSampler, mTexture.GetTex());
        
        glm::mat4 WorldMat = glm::scale(glm::mat4(1), glm::vec3(20, 20, 20));
        WorldMat = glm::translate(WorldMat, glm::vec3(-2, 1, 0));
        
        GLint gWorldMat = glGetUniformLocation(mAmbientProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mAmbientProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mAmbientProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(WorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mCube.Render();
    }
    
    glUseProgram(0);
}