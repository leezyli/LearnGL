//
//  SSAO.h
//  LearnCG
//
//  Created by leezy on 16/7/2.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__SSAO__
#define __LearnCG__SSAO__

#include <stdio.h>
#include "LearnCG.h"
#include "Camera.h"
#include "Pipeline.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "AODepth.h"
#include "AOTexture.h"

#define MAX_KERNEL_SIZE 64

class SSAO
{
    typedef SSAO this_class;
public:
    SSAO();
    ~SSAO();
    
    bool Init();
    void RunLoop();
protected:
    static Camera gCamera;
    static Pipeline gPipeline;
    static SSAO* gThisDemo;
    static void OnCallbackKeyEvnt(GLFWwindow* pWindow, int key, int scancode, int action, int mode);
    static void OnFramebufferSize(GLFWwindow* pWindow, int width, int height);
protected:
    bool LoadGeomShader();
    bool LoadSSAOShader();
    bool LoadLightShader();
    void ChangeRender();
    void OnRender();
    void RenderGeomPass();
    void RenderSSAOPass();
    void RenderLightPass();
private:
    GLuint mVAO;
    GLuint mGeomProg;
    GLuint mSSAOProg;
    GLuint mLightProg;
    GLfloat mSampleRad;
    glm::vec3 mSamplePos[MAX_KERNEL_SIZE];
    bool mEnableSSAO;
    
    Model mQuad;
    Model mModel;
    glm::mat4 mModelMat;
    
    AODepth mDepthTex;
    AOTexture mAOTex;
    DirectionalLight mLight;
    
    GLint mWidth;
    GLint mHeight;
    GLFWwindow* mpWindow;
};

#endif /* defined(__LearnCG__SSAO__) */
