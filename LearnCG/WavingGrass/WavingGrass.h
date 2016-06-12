//
//  WavingGrass.h
//  LearnCG
//
//  Created by leezy on 16/5/24.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__WavingGrass__
#define __LearnCG__WavingGrass__

#include <stdio.h>
#include "LearnCG.h"
#include "Camera.h"
#include "Pipeline.h"
#include "Texture2D.h"
#include <vector>

class WavingGrass
{
    typedef WavingGrass this_class;
public:
    WavingGrass();
    ~WavingGrass();
    
    bool Init();
    void RunLoop();
protected:
    static Camera gCamera;
    static Pipeline gPipeline;
    static void OnFramebufferSize(GLFWwindow* pWindow, int width, int height);
protected:
    void OnRender();
    bool LoadGrassShader();
private:
    GLFWwindow* mpWindow;

    int n,m; // n row, m col
    int w,h;
    glm::vec3 mUp, mRight;
    GLuint mFinProg;
    GLuint mVAO;
    GLuint mVBO;
    GLuint mIBO;
    Texture2D mGrassTex;
};

#endif /* defined(__LearnCG__WavingGrass__) */
