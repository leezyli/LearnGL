//
//  ShadowVolume.h
//  LearnCG
//
//  Created by leezy on 16/5/29.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__ShadowVolume__
#define __LearnCG__ShadowVolume__

#include <stdio.h>
#include "LearnCG.h"
#include "Model.h"
#include "Camera.h"
#include "Pipeline.h"
#include "DirectionalLight.h"
#include "SVDepthTexture.h"
#include "Texture2D.h"
#include "Cube.h"

class ShadowVolume
{
    typedef ShadowVolume this_class;
public:
    ShadowVolume();
    ~ShadowVolume();
    
    bool Init();
    void RunLoop();
protected:
    static Camera gCamera;
    static Pipeline gPipeline;
    static void OnFramebufferSize(GLFWwindow* pWindow, int width, int height);
protected:
    bool LoadDepthShader();
    bool LoadStencilShader();
    bool LoadLightShader();
    bool LoadAmbientShader();
    void OnRender();
    void RenderForGeom();
    void RenderForStencil();
    void RenderForLight();
    void RenderForAmbient();
private:
    GLFWwindow* mpWindow;
    typedef struct {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
    } Vertex;
    GLuint mGemProg;
    GLuint mSteProg;
    GLuint mLightProg;
    GLuint mAmbientProg;
    GLuint mVAO;
    GLuint mVBO;
    GLuint mIBO;
    Model mModel;
    glm::mat4 mWorldMat;
    DirectionalLight mDirectLight;
    Texture2D mTexture, mTexture2;
    SVDepthTexture mDepthTex;
    
    Cube mCube;
};


#endif /* defined(__LearnCG__ShadowVolume__) */
