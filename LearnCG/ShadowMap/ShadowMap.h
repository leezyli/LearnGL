//
//  ShadowMap.h
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__ShadowMap__
#define __LearnCG__ShadowMap__

#include <stdio.h>
#include "LearnCG.h"
#include "Model.h"
#include "Camera.h"
#include "Pipeline.h"
#include "SMDepthTexture.h"
#include "DistanceTexture.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"

class ShadowMapDemo
{
    typedef ShadowMapDemo this_class;
public:
    ShadowMapDemo();
    ~ShadowMapDemo();
    
    bool Init();
    void RunLoop();
protected:
    static Camera gCamera;
    static Pipeline gPipeline;
    static void OnWindowSize(GLFWwindow *pWindow, int width, int height);
    static void OnFramebufferSize(GLFWwindow *pWindow, int width, int height);
protected:
    bool LoadShadowShader();
    bool LoadDepthShader();
    bool LoadDistanceShader();
    void OnRender();
    void RenderShadowPass_SpotLight();
    void RenderFinalPass_SpotLight();
    void RenderShadowPass_PointLight();
    void RenderFinalPass_PointLight();
    void RenderShadowPass_DirectLight();
    void RenderFinalPass_DirectLight();
private:
    GLFWwindow* mpWindow;
    typedef struct {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
    } Vertex;
    GLuint mFinProg;
    GLuint mDepthProg;
    GLuint mDistProg;
    GLuint mVAO;
    GLuint mVBO;
    GLuint mIBO;
    Model mModel;
    glm::mat4 mWorldMat;
    Camera mLightCamera;
    Pipeline mLightPipeline;
    Texture2D mTexture;
    SMDepthTexture mDepthTex;
    DistanceTexture mDistTex;
    SpotLight mSpotLight;
    PointLight mPointLight;
    DirectionalLight mDirectLight;
};

#endif /* defined(__LearnCG__ShadowMap__) */
