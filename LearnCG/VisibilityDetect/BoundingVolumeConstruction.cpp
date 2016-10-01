//
//  BoundingVolumeConstruction.cpp
//  LearnCG
//
//  Created by leezy on 16/8/7.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "BoundingVolumeConstruction.h"
#include "UtilCG.h"


Camera BoundingVolumeConstruction::gCamera;
Pipeline BoundingVolumeConstruction::gPipeline;

BoundingVolumeConstruction::BoundingVolumeConstruction()
{
    mProg = INVALID_HANDLE;
}

BoundingVolumeConstruction::~BoundingVolumeConstruction()
{
    if ( INVALID_HANDLE != mProg )
        glDeleteProgram(mProg);
}

bool BoundingVolumeConstruction::Init()
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
    //glfwSetKeyCallback(mpWindow, OnCallbackKeyEvnt);
    glfwSetFramebufferSizeCallback(mpWindow, OnFramebufferSize);
    glfwGetFramebufferSize(mpWindow, &mWidth, &mHeight);
    
    if ( !LoadShader() )
        return false;
    
    if ( !mModel.Load(g_AssetsPath+"Assets/Monster/yanmo.obj"))
        return false;
    
    gCamera.LookAt(glm::vec3(80, 80, -50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    gPipeline.SetPerspective(60, (GLfloat)mWidth/mHeight, 1, 1500);
    return true;

}

void BoundingVolumeConstruction::OnFramebufferSize(GLFWwindow *pWindow, int width, int height)
{
    
}

bool BoundingVolumeConstruction::LoadShader()
{
    std::string VShader = g_AssetsPath+"/VisibilityDetect/Volume.vs";
    std::string FShader = g_AssetsPath+"/VisibilityDetect/Volume.fs";
    mProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return mProg != INVALID_HANDLE;
}

void BoundingVolumeConstruction::RunLoop()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    mModelMat = glm::scale(glm::mat4(1), glm::vec3(2, 2, 2));
    mModelMat = glm::translate(mModelMat, glm::vec3(0, 0, 10));

    BoundingBox box;
    mModel.MakeBoundingBox(box);
    mBoxDrawer.Make(box);
    
    while (!glfwWindowShouldClose(mpWindow)) {
        RenderVolume();
        glfwSwapBuffers(mpWindow);
        glfwPollEvents();
    }
    
    glfwTerminate();
}

void BoundingVolumeConstruction::RenderVolume()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mProg);
    
    GLint gSampler = glGetUniformLocation(mProg, "gSampler");
    glUniform1i(gSampler, 0);
    
    GLint gWorldMat = glGetUniformLocation(mProg, "gWorldMat");
    GLint gViewMat = glGetUniformLocation(mProg, "gViewMat");
    GLint gProjMat = glGetUniformLocation(mProg, "gProjMat");
    glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
    glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
    glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
    
    mModel.Render();
    glUseProgram(0);
    
    mBoxDrawer.Render(gCamera, gPipeline);
}