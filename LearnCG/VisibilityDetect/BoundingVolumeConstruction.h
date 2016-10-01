//
//  BoundingVolumeConstruction.h
//  LearnCG
//
//  Created by leezy on 16/8/7.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__BoundingVolumeConstruction__
#define __LearnCG__BoundingVolumeConstruction__

#include <stdio.h>
#include "Model.h"
#include "Math.h"
#include "BoundingVolume.h"
#include "Camera.h"
#include "Pipeline.h"


class BoundingVolumeConstruction
{
    typedef BoundingVolumeConstruction this_class;
public:
    
    BoundingVolumeConstruction();
    ~BoundingVolumeConstruction();
    
    bool Init();
    void RunLoop();
protected:
    static Camera gCamera;
    static Pipeline gPipeline;
    static void OnFramebufferSize(GLFWwindow* pWindow, int width, int height);
protected:
    bool LoadShader();
    void RenderVolume();
private:
    GLuint mVAO;
    GLuint mProg;
    Model mModel;
    glm::mat4 mModelMat;
    GLFWwindow* mpWindow;
    BoundingBoxDrawer mBoxDrawer;
    int mWidth, mHeight;
};

#endif /* defined(__LearnCG__BoundingVolumeConstruction__) */
