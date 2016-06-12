//
//  EffectWaterDemo.h
//  LearnCG
//
//  Created by leezy on 16/4/26.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__EffectWaterDemo__
#define __LearnCG__EffectWaterDemo__

#include <stdio.h>
#include "LearnCG.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Pipeline.h"

#define MAX_WAVE_NUM 2

class EffectWaterDemo
{
public:
    EffectWaterDemo();
    ~EffectWaterDemo();
    
    bool Init();
    void RunLoop();
public:
    static Camera gCamera;
    static Pipeline gPipeline;
    static void OnWindowSize(GLFWwindow* pWindow, int width, int height);
    static void OnFramebufferSize(GLFWwindow* pWindow, int width, int height);
protected:
    bool LoadShader();
    void IntWave();
    void GenMesh();
    void OnRender();
private:
    GLFWwindow* mpWindow;
    
    GLuint mVAO;
    GLuint mVBO;
    GLuint mTBO;
    GLuint mIBO;
    GLuint mProg;
    
    int n , m;
    Texture2D mTexture0;
    Texture2D mTexture1;
    
    struct DirectionalWave
    {
        float L; //波长
        float A; //振幅
        float W; //频率
        float S; //速度
        float P; //相位
        float Q; //控制参数
        glm::vec2 Dir;//方向向量
    };
    DirectionalWave mWaves[MAX_WAVE_NUM];
};

#endif /* defined(__LearnCG__EffectWaterDemo__) */
