//
//  ShadowPCF.h
//  LearnCG
//
//  Created by leezy on 16/6/23.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__ShadowPCF__
#define __LearnCG__ShadowPCF__

#include <stdio.h>
#include "LearnCG.h"
#include "Pipeline.h"
#include "Camera.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "PCFTexture.h"

class ShadowPCF
{
    typedef ShadowPCF this_class;
public:
    
    ShadowPCF();
    ~ShadowPCF();
    
    bool Init();
    void RunLoop();
    void ChangeRenderMode();
protected:
    static Camera gCamera;
    static Pipeline gPipeline;
    static ShadowPCF* gThisDemo;
    static void OnFramebufferSize(GLFWwindow* pWindow, int width, int height);
    static void OnCallbackKeyEvnt(GLFWwindow* pWindow, int key, int scancode, int action, int mode);
protected:
    bool LoadDepthShader();
    bool LoadLightShader();
    bool LoadLightShader_PCF();
    void OnRender();
    void RenderDepthPass_DirectLight();
    void RenderLightPass_DirectLight();
    void RenderLightPass_DirectLight_PCF();
private:
    GLFWwindow* mpWindow;
    
    bool mEnablePCF;
    GLuint mDepthProg;
    GLuint mLightProg;
    GLuint mPCFProg;
    
    GLuint mVAO;
    Model mMonster;
    DirectionalLight mLight;
    
    glm::mat4 mMonsterMat;
    glm::mat4 mGroundMat;
    Texture2D mGroundTex;
    PCFTexture mDepthTex;
    
    Camera mLightCame;
    Pipeline mLightPipe;
};

#endif /* defined(__LearnCG__ShadowPCF__) */
