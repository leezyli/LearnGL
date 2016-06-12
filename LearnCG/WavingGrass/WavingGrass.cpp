//
//  WavingGrass.cpp
//  LearnCG
//
//  Created by leezy on 16/5/24.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "WavingGrass.h"


Camera WavingGrass::gCamera;
Pipeline WavingGrass::gPipeline;

WavingGrass::WavingGrass()
{
    mFinProg = 0;
    mVAO = 0;
    mVBO = 0;
    mIBO = 0;
}

WavingGrass::~WavingGrass()
{
    if ( mFinProg )
        glDeleteProgram(mFinProg);
    if ( mIBO )
        glDeleteBuffers(1, &mIBO);
    if ( mVBO )
        glDeleteBuffers(1, &mVBO);
    if ( mVAO )
        glDeleteBuffers(1, &mVAO);
}

bool WavingGrass::Init()
{
    glfwSetErrorCallback(glfwErrorCallback_GPU);
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    mpWindow = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "WavingGrass for LearnCG", nullptr, nullptr);
    if ( !mpWindow )
    {
        GPU_Log("WavingGrass::Init failed!%d", 1);
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(mpWindow);
    glfwSetFramebufferSizeCallback(mpWindow, OnFramebufferSize);
    
    if ( !LoadGrassShader() )
        return false;
    
    if ( !mGrassTex.Load(g_AssetsPath+"Assets/Grass/grass_PNG4922.png"))
        return false;
    
    int width, height;
    glfwGetFramebufferSize(mpWindow, &width, &height);
    
    glViewport(0, 0, width, height);
    
    gCamera.LookAt(glm::vec3(0, 50, 120), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    gPipeline.SetPerspective(90, (GLfloat)width/height, 1, 500);
    return true;
}

void WavingGrass::OnFramebufferSize(GLFWwindow *pWindow, int width, int height)
{
    glViewport(0, 0, width, height);
    gPipeline.SetPerspective(90, (GLfloat)width/height, 1, 500);
}

bool WavingGrass::LoadGrassShader()
{
    std::string VShader = g_AssetsPath+"WavingGrass/Grass.vs";
    std::string FShader = g_AssetsPath+"WavingGrass/Grass.fs";
    std::string GShader = g_AssetsPath+"WavingGrass/Grass.gs";
    mFinProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str(), GShader.c_str());
    return mFinProg != INVALID_HANDLE;
}

void WavingGrass::RunLoop()
{
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    n = 20; m = 10; w = 20, h = 50;
    mUp = glm::vec3(0, 1, 0);
    mRight = glm::vec3(1, 0, 0);
    
    std::vector<glm::vec3> grass(n*m);
    
    int Z = -50, X = -100;
    for (int i = 0; i < n; ++i, Z+=5) {
        int offset = (i%2==0)?X:(X+10);
        for (int j = 0; j < m; ++j, offset+=w) {
            float x = (2*offset+w)/2;//(offsetX+offsetX+w)/2
            float y = h/2;
            grass[m*i+j] = glm::vec3(x, y, Z); //Quads的中心点
        }
    }
    
    std::vector<GLushort> indices(n*m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            indices[m*i+j] = j;
        }
    }
    
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*grass.size(), &grass[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &mIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indices.size(), &indices[0], GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glfwSetTime(0.2);
    while ( !glfwWindowShouldClose(mpWindow) ) {
        OnRender();
        glfwSwapBuffers(mpWindow);
        glfwPollEvents();
    }
    
    glfwTerminate();
}


void WavingGrass::OnRender()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mFinProg);
    
    mGrassTex.Bind(GL_TEXTURE0);
    GLint gSampler = glGetUniformLocation(mFinProg, "gSampler");
    glUniform1i(gSampler, mGrassTex.GetTex());
    
    GLint gAngle = glGetUniformLocation(mFinProg, "gAngle");
    GLint gHalfWidth = glGetUniformLocation(mFinProg, "gHalfWidth");
    GLint gHalfHeight = glGetUniformLocation(mFinProg, "gHalfHeight");
    glUniform1f(gAngle, glm::pi<float>()/3);
    glUniform1f(gHalfWidth, ((float)w)/2);
    glUniform1f(gHalfHeight, ((float)h)/2);
    
    GLint gUp = glGetUniformLocation(mFinProg, "gUp");
    GLint gRight = glGetUniformLocation(mFinProg, "gRight");
    glUniform3fv(gUp, 1, glm::value_ptr(mUp));
    glUniform3fv(gRight, 1, glm::value_ptr(mRight));
    
    GLint gTimeStamp = glGetUniformLocation(mFinProg, "gTimeStamp");
    GLint gWindDirection = glGetUniformLocation(mFinProg, "gWindDirection");
    GLint gWindStrength = glGetUniformLocation(mFinProg, "gWindStrength");
    glUniform3fv(gWindDirection, 1, glm::value_ptr(glm::vec3(1, 1, -1)));
    glUniform1f(gWindStrength, 1);
    glUniform1f(gTimeStamp, glfwGetTime());
    
    GLint gWorldMat = glGetUniformLocation(mFinProg, "gWorldMat");
    GLint gViewMat = glGetUniformLocation(mFinProg, "gViewMat");
    GLint gProjMat = glGetUniformLocation(mFinProg, "gProjMat");
    glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
    glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(gCamera.GetViewMat()));
    glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(gPipeline.GetProjMat()));
    
    glBindVertexArray(mVAO);
    for (int i = 0; i < n; ++i) {
        glDrawElementsBaseVertex(GL_POINTS, m, GL_UNSIGNED_SHORT, (GLvoid*)(m*i*sizeof(GLushort)), m*i);
    }
    glBindVertexArray(0);
    glUseProgram(0);
}
