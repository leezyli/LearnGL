//
//  ShadowMap.cpp
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "ShadowMap.h"

Camera ShadowMapDemo::gCamera;
Pipeline ShadowMapDemo::gPipeline;

ShadowMapDemo::ShadowMapDemo()
{
    mFinProg = 0;
    mDistProg = 0;
    mDepthProg = 0;
    mVAO = 0;
    mVBO = 0;
    mIBO = 0;
}

ShadowMapDemo::~ShadowMapDemo()
{
    if ( mFinProg )
        glDeleteProgram(mFinProg);
    if ( mDepthProg )
        glDeleteProgram(mDepthProg);
    if ( mDistProg )
        glDeleteProgram(mDistProg);
    if ( mIBO )
        glDeleteBuffers(1, &mIBO);
    if ( mVBO )
        glDeleteBuffers(1, &mVBO);
    if ( mVAO )
        glDeleteVertexArrays(1, &mVAO);
}

bool ShadowMapDemo::Init()
{
    glfwSetErrorCallback(glfwErrorCallback_GPU);
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    mpWindow = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "ShadowMap for LearnCG", nullptr, nullptr);
    if ( !mpWindow )
    {
        GPU_Log("ShadowMapDemo::Init failed!%d", 1);
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(mpWindow);
    glfwSetWindowSizeCallback(mpWindow, OnWindowSize);
    glfwSetFramebufferSizeCallback(mpWindow, OnFramebufferSize);

    int width, height;
    glfwGetFramebufferSize(mpWindow, &width, &height);
   
    if ( !mDepthTex.Load(width, height) )
        return false;
    
    if ( !mDistTex.Load(std::max(width, height)) )
        return false;
    
    if ( !LoadShadowShader() )
        return false;
    
    if ( !LoadDepthShader() )
        return false;
    
    if ( !LoadDistanceShader() )
        return false;
    
    if ( !mModel.Load(g_AssetsPath+"Assets/Monster/yanmo.obj") )
        return false;
    
    if ( !mTexture.Load(g_AssetsPath+"Assets/diffuse.jpg") )
        return false;
 
    glViewport(0, 0, width, height);
    
    gCamera.LookAt(glm::vec3(150, 150, 150), glm::vec3(0), glm::vec3(0,1,0));
    gPipeline.SetPerspective(45, (GLfloat)width/height, 1, 500);

    return true;
}

void ShadowMapDemo::OnWindowSize(GLFWwindow *pWindow, int width, int height)
{
    
}

void ShadowMapDemo::OnFramebufferSize(GLFWwindow *pWindow, int width, int height)
{
    //mDepthTex.Load(width, height);
    glViewport(0, 0, width, height);
    gPipeline.SetPerspective(45, (GLfloat)width/height, 1, 500);
}

bool ShadowMapDemo::LoadShadowShader()
{
    std::string VShader = g_AssetsPath+"ShadowMap/Shadow.vs";
    std::string FShader = g_AssetsPath+"ShadowMap/Shadow.fs";
    mFinProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mFinProg != INVALID_HANDLE;
}

bool ShadowMapDemo::LoadDepthShader()
{
    std::string VShader = g_AssetsPath+"ShadowMap/Depth.vs";
    std::string FShader = g_AssetsPath+"ShadowMap/Depth.fs";
    mDepthProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mDepthProg != INVALID_HANDLE;
}

bool ShadowMapDemo::LoadDistanceShader()
{
    std::string VShader = g_AssetsPath+"ShadowMap/Distance.vs";
    std::string FShader = g_AssetsPath+"ShadowMap/Distance.fs";
    mDistProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mDistProg != INVALID_HANDLE;
}

void ShadowMapDemo::RunLoop()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    std::vector<Vertex> Vertices(4);
    Vertices[0].Position = glm::vec3(-150, 0, -150);
    Vertices[0].Normal = glm::vec3(0, 1, 0);
    Vertices[0].TexCoord = glm::vec2(0, 0);
    Vertices[1].Position = glm::vec3(-150, 0, 150);
    Vertices[1].Normal = glm::vec3(0, 1, 0);
    Vertices[1].TexCoord = glm::vec2(0, 1);
    Vertices[2].Position = glm::vec3(150, 0, -150);
    Vertices[2].Normal = glm::vec3(0, 1, 0);
    Vertices[2].TexCoord = glm::vec2(1, 0);
    Vertices[3].Position = glm::vec3(150, 0, 150);
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
    
    // Spot Light
    mSpotLight.SetLightColor(glm::vec4(1, 1, 0.3, 1));
    mSpotLight.SetLightIntensity(2);
    mSpotLight.SetAmbientColor(glm::vec4(1, 1, 1, 1));
    mSpotLight.SetAmbientIntensity(1);
    mSpotLight.SetLightPos(glm::vec3(-50, 150, 80));
    mSpotLight.SetExp(0.0001);
    mSpotLight.SetLinear(0.0001);
    mSpotLight.SetConstant(0.1);
    mSpotLight.SetCastPos(glm::vec3(0, 0, 0));
    mSpotLight.SetCastAngle(60);
    
    // Point Light
    mPointLight.SetLightColor(glm::vec4(0.2, 0.8, 1, 1));
    mPointLight.SetLightIntensity(2);
    mPointLight.SetAmbientColor(glm::vec4(1, 1, 1, 1));
    mPointLight.SetAmbientIntensity(1);
    mPointLight.SetLightPos(glm::vec3(-40, 120, 100));
    mPointLight.SetExp(0.0001);
    mPointLight.SetLinear(0.0001);
    mPointLight.SetConstant(0.1);
    
    // Directional Light
    mDirectLight.SetLightColor(glm::vec4(1, 1, 1, 1));
    mDirectLight.SetLightIntensity(1);
    mDirectLight.SetAmbientColor(glm::vec4(1, 1, 1, 1));
    mDirectLight.SetAmbientIntensity(1);
    mDirectLight.SetLightPos(glm::vec3(-100, 100, 50));
    mDirectLight.SetCastPos(glm::vec3(-50, 0, -100));
    
    // 模型的世界坐标空间
    mWorldMat = glm::scale(glm::mat4(1), glm::vec3(2, 2, 2));
    mWorldMat = glm::translate(mWorldMat, glm::vec3(0, 0, 20));
    
    while ( !glfwWindowShouldClose(mpWindow) ) {
        OnRender();
        glfwSwapBuffers(mpWindow);
        glfwPollEvents();
    }
    
    glfwTerminate();
}

void ShadowMapDemo::OnRender()
{
    // glfw没有菜单能力, 简单的通过注释来切换不同的光源
    
    //RenderShadowPass_SpotLight();
    //RenderFinalPass_SpotLight();
    
    RenderShadowPass_PointLight();
    RenderFinalPass_PointLight();
    
    //RenderShadowPass_DirectLight();
    //RenderFinalPass_DirectLight();
}

void ShadowMapDemo::RenderShadowPass_SpotLight()
{
    mDepthTex.BindForDraw();
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // z-flighting处理
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.1, 0);
    
    glUseProgram(mDepthProg);
    
    GLint Viewport[4] = {0};
    glGetIntegerv(GL_VIEWPORT, Viewport);
    
    // 将相机移到光源
    // 不同的透视投影矩阵, 对阴影会有影响(例如对象被裁剪, 阴影采样就会不正确)
    mLightCamera.LookAt(mSpotLight.GetLightPos(), mSpotLight.GetCastPos(), gCamera.GetUp());
    mLightPipeline.SetPerspective(90, (GLfloat)Viewport[2]/Viewport[3], 1, 500);
    
    {
        GLint gWorldMat = glGetUniformLocation(mDepthProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mDepthProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mDepthProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mWorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(mLightCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(mLightPipeline.GetProjMat()));
        
        mModel.Render();
    }
    
    {
        GLint gWorldMat = glGetUniformLocation(mDepthProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mDepthProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mDepthProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(mLightCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(mLightPipeline.GetProjMat()));
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
    
    glUseProgram(0);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void ShadowMapDemo::RenderFinalPass_SpotLight()
{
    mDepthTex.BindForRead(GL_TEXTURE1);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mFinProg);
    
    GLint gShadowMap = glGetUniformLocation(mFinProg, "gShadowMap");
    glUniform1i(gShadowMap, mDepthTex.GetTex());
    
    GLint gLightProjMat = glGetUniformLocation(mFinProg, "gLightProjMat");
    glm::mat4 LightProjMat = mLightPipeline.GetProjMat() * mLightCamera.GetViewMat();
    glUniformMatrix4fv(gLightProjMat, 1, GL_FALSE, glm::value_ptr(LightProjMat));
    
    // Spot Light
    GLint LightColor = glGetUniformLocation(mFinProg, "gSpotLight.Light.Base.LightColor");
    GLint AmbientColor = glGetUniformLocation(mFinProg, "gSpotLight.Light.Base.AmbientColor");
    GLint Shiness = glGetUniformLocation(mFinProg, "gSpotLight.Light.Base.Shiness");
    GLint LightPos = glGetUniformLocation(mFinProg, "gSpotLight.Light.Base.LightPos");
    GLint Exp = glGetUniformLocation(mFinProg, "gSpotLight.Light.Exp");
    GLint Linear = glGetUniformLocation(mFinProg, "gSpotLight.Light.Linear");
    GLint Constant = glGetUniformLocation(mFinProg, "gSpotLight.Light.Constant");
    GLint LightCastPos = glGetUniformLocation(mFinProg, "gSpotLight.LightCastPos");
    GLint CutOff = glGetUniformLocation(mFinProg, "gSpotLight.CutOff");
    glUniform4fv(LightColor, 1, glm::value_ptr(mSpotLight.GetLightColor()));
    glUniform4fv(AmbientColor, 1, glm::value_ptr(mSpotLight.GetAmbientColor()));
    glUniform1f(Shiness, 128);
    glUniform3fv(LightPos, 1, glm::value_ptr(mSpotLight.GetLightPos()));
    glUniform1f(Exp, mSpotLight.GetExp());
    glUniform1f(Linear, mSpotLight.GetLinear());
    glUniform1f(Constant, mSpotLight.GetConstant());
    glUniform3fv(LightCastPos, 1, glm::value_ptr(mSpotLight.GetCastPos()));
    glUniform1f(CutOff, cosf(mSpotLight.GetCastAngel()));
    
    GLint gCameraPos = glGetUniformLocation(mFinProg, "gCameraPos");
    glUniform3fv(gCameraPos, 1, glm::value_ptr(gCamera.GetEyePos()));
    
    {
        mTexture.Bind(GL_TEXTURE0);
        GLint gSampler = glGetUniformLocation(mFinProg, "gSampler");
        glUniform1i(gSampler, mTexture.GetTex());
        
        GLint gWolrdMat = glGetUniformLocation(mFinProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mFinProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mFinProg, "gProjMat");
        glUniformMatrix4fv(gWolrdMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);
        glBindVertexArray(0);
    }
    
    {
        GLint gSampler = glGetUniformLocation(mFinProg, "gSampler");
        glUniform1i(gSampler, 0);
        
        GLint gWolrdMat = glGetUniformLocation(mFinProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mFinProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mFinProg, "gProjMat");
        glUniformMatrix4fv(gWolrdMat, 1, GL_FALSE, glm::value_ptr(mWorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mModel.Render();
    }
    
    glUseProgram(0);
}

void ShadowMapDemo::RenderShadowPass_PointLight()
{
    // 由于点光源没有用到深度缓冲采样, 所以polygon offset的作用不大
    glUseProgram(mDistProg);
    
    GLint gLightPos = glGetUniformLocation(mDistProg, "gLightPos");
    glUniform3fv(gLightPos, 1, glm::value_ptr(mPointLight.GetLightPos()));
    
    GLint Viewport[4] = {0};
    glGetIntegerv(GL_VIEWPORT, Viewport);
    
    GLint MaxEdge = std::max(Viewport[2], Viewport[3]);
    glViewport(0, 0, MaxEdge, MaxEdge);
    
    // 不同的透视投影矩阵, 对阴影会有影响(例如对象被裁剪, 阴影采样就会不正确)
    mLightPipeline.SetPerspective(90, (GLfloat)1.0, 1, 500);
    
    // 将相机移到光源
    glm::vec3 Up[6] = {
        glm::vec3(0, -1, 0),// pos_x
        glm::vec3(0, -1, 0),// neg_x
        glm::vec3(0, 0, 1),// pos_y
        glm::vec3(0, 0, -1),// neg_y
        glm::vec3(0, -1, 0),// pos_z
        glm::vec3(0, -1, 0)// neg_z
    };
    glm::vec3 Center[6] = {
        glm::vec3(1, 0, 0),// pos_x
        glm::vec3(-1, 0, 0),// neg_x
        glm::vec3(0, 1, 0),// pos_y
        glm::vec3(0, -1, 0),// neg_y
        glm::vec3(0, 0, 1),// pos_z
        glm::vec3(0, 0, -1)// neg_z
    };
    for (int i = 0; i < 6; ++i) {
        Center[i] += mPointLight.GetLightPos();
    }
    
    for (int i = 0; i < 6; ++i) {
        mLightCamera.LookAt(mPointLight.GetLightPos(), Center[i], Up[i]);
        
        mDistTex.BindForDraw(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        {
            GLint gWorldMat = glGetUniformLocation(mDistProg, "gWorldMat");
            GLint gViewMat = glGetUniformLocation(mDistProg, "gViewMat");
            GLint gProjMat = glGetUniformLocation(mDistProg, "gProjMat");
            glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mWorldMat));
            glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(mLightCamera.GetViewMat()));
            glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(mLightPipeline.GetProjMat()));
            
            mModel.Render();
        }
        
        {
            GLint gWorldMat = glGetUniformLocation(mDistProg, "gWorldMat");
            GLint gViewMat = glGetUniformLocation(mDistProg, "gViewMat");
            GLint gProjMat = glGetUniformLocation(mDistProg, "gProjMat");
            glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
            glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(mLightCamera.GetViewMat()));
            glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(mLightPipeline.GetProjMat()));
            
            glBindVertexArray(mVAO);
            glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
            glBindVertexArray(0);
        }
    }
    
    glUseProgram(0);
    glViewport(0, 0, Viewport[2], Viewport[3]);
}

void ShadowMapDemo::RenderFinalPass_PointLight()
{
    mDistTex.BindForRead(GL_TEXTURE1);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mFinProg);
    
    GLint gDistanceMap = glGetUniformLocation(mFinProg, "gDistanceMap");
    glUniform1i(gDistanceMap, mDistTex.GetTex());
    
    GLint LightColor = glGetUniformLocation(mFinProg, "gPointLight.Base.LightColor");
    GLint AmbientColor = glGetUniformLocation(mFinProg, "gPointLight.Base.AmbientColor");
    GLint Shiness = glGetUniformLocation(mFinProg, "gPointLight.Base.Shiness");
    GLint LightPos = glGetUniformLocation(mFinProg, "gPointLight.Base.LightPos");
    GLint Exp = glGetUniformLocation(mFinProg, "gPointLight.Exp");
    GLint Linear = glGetUniformLocation(mFinProg, "gPointLight.Linear");
    GLint Constant = glGetUniformLocation(mFinProg, "gPointLight.Constant");
    glUniform4fv(LightColor, 1, glm::value_ptr(mPointLight.GetLightColor()));
    glUniform4fv(AmbientColor, 1, glm::value_ptr(mPointLight.GetAmbientColor()));
    glUniform1f(Shiness, 128);
    glUniform3fv(LightPos, 1, glm::value_ptr(mPointLight.GetLightPos()));
    glUniform1f(Exp, mPointLight.GetExp());
    glUniform1f(Linear, mPointLight.GetLinear());
    glUniform1f(Constant, mPointLight.GetConstant());
    
    GLint gCameraPos = glGetUniformLocation(mFinProg, "gCameraPos");
    glUniform3fv(gCameraPos, 1, glm::value_ptr(gCamera.GetEyePos()));
    
    {
        mTexture.Bind(GL_TEXTURE0);
        GLint gSampler = glGetUniformLocation(mFinProg, "gSampler");
        glUniform1i(gSampler, mTexture.GetTex());
        
        GLint gWorldMat = glGetUniformLocation(mFinProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mFinProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mFinProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);
        glBindVertexArray(0);
    }
    
    {
        GLint gSampler = glGetUniformLocation(mFinProg, "gSampler");
        glUniform1i(gSampler, 0);
        
        GLint gWorldMat = glGetUniformLocation(mFinProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mFinProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mFinProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mWorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mModel.Render();
    }
    
    glUseProgram(0);
}

void ShadowMapDemo::RenderShadowPass_DirectLight()
{
    mDepthTex.BindForDraw();
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mDepthProg);
    
    // z-flighting 处理
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.004, 0);
    
    GLint Viewport[4] = {0};
    glGetIntegerv(GL_VIEWPORT, Viewport);
   
    // 把相机移到光源位置
    mLightCamera.LookAt(mDirectLight.GetLightPos(), mDirectLight.GetCastPos(), gCamera.GetUp());
    mLightPipeline.SetOrth(-150, 150, -150, 150, -50, 500);
    //mLightPipeline.SetOrth(-Viewport[2]/4, Viewport[2]/4, -Viewport[3]/4, Viewport[3]/4, -50, 500);
    {
        GLint gWorldMat = glGetUniformLocation(mDepthProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mDepthProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mDepthProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mWorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(mLightCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(mLightPipeline.GetProjMat()));
        
        mModel.Render();
    }
    
    {
        GLint gWorldMat = glGetUniformLocation(mDepthProg, "gWorldMat");
        GLint gViewMat = glGetUniformLocation(mDepthProg, "gViewMat");
        GLint gProjMat = glGetUniformLocation(mDepthProg, "gProjMat");
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(mLightCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(mLightPipeline.GetProjMat()));
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
    
    glUseProgram(0);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void ShadowMapDemo::RenderFinalPass_DirectLight()
{
    mDepthTex.BindForRead(GL_TEXTURE1);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mFinProg);
    
    GLint gShadowMap = glGetUniformLocation(mFinProg, "gShadowMap");
    glUniform1i(gShadowMap, mDepthTex.GetTex());
    
    GLint gLightProjMat = glGetUniformLocation(mFinProg, "gLightProjMat");
    glm::mat4 LightProjMat = mLightPipeline.GetProjMat()*mLightCamera.GetViewMat();
    glUniformMatrix4fv(gLightProjMat, 1, GL_FALSE, glm::value_ptr(LightProjMat));
    
    // Directional Light
    GLint LightColor = glGetUniformLocation(mFinProg, "gDirectLight.Base.LightColor");
    GLint AmbientColor = glGetUniformLocation(mFinProg, "gDirectLight.Base.AmbientColor");
    GLint Shiness = glGetUniformLocation(mFinProg, "gDirectLight.Base.Shiness");
    GLint LightPos = glGetUniformLocation(mFinProg, "gDirectLight.Base.LightPos");
    GLint LightToPos = glGetUniformLocation(mFinProg, "gDirectLight.LightToPos");
    glUniform4fv(LightColor, 1, glm::value_ptr(mDirectLight.GetLightColor()));
    glUniform4fv(AmbientColor, 1, glm::value_ptr(mDirectLight.GetAmbientColor()));
    glUniform1f(Shiness, 128);
    glUniform3fv(LightPos, 1, glm::value_ptr(mDirectLight.GetLightPos()));
    glUniform3fv(LightToPos, 1, glm::value_ptr(mDirectLight.GetCastPos()));
    
    GLint gCameraPos = glGetUniformLocation(mFinProg, "gCameraPos");
    glUniform3fv(gCameraPos, 1, glm::value_ptr(gCamera.GetEyePos()));
    
    GLint gWorldMat = glGetUniformLocation(mFinProg, "gWorldMat");
    GLint gViewMat = glGetUniformLocation(mFinProg, "gViewMat");
    GLint gProjMat = glGetUniformLocation(mFinProg, "gProjMat");
    
    {
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(mWorldMat));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        GLint gSampler = glGetUniformLocation(mFinProg, "gSampler");
        glUniform1i(gSampler, 0);
        mModel.Render();
    }
    
    {
        glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
        glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
        glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
        
        mTexture.Bind(GL_TEXTURE0);
        GLint gSampler = glGetUniformLocation(mFinProg, "gSampler");
        glUniform1i(gSampler, mTexture.GetTex());
        
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
    
    
    glUseProgram(0);
}
