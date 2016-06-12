//
//  SVDepthTexture.h
//  LearnCG
//
//  Created by leezy on 16/6/3.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__SVDepthTexture__
#define __LearnCG__SVDepthTexture__

#include <stdio.h>
#include "LearnCG.h"

class SVDepthTexture
{
    typedef SVDepthTexture this_class;
public:
    SVDepthTexture();
    ~SVDepthTexture();
    
    bool Load(int width, int height);
    void BindForDraw();
    void BindForRead(GLenum eTexture);
    GLint GetTex() const;
protected:
    bool InitFBO();
private:
    bool mInit;
    GLuint mFBO;
    GLuint mDepthTex;
    GLint mSampler;
    GLint mWidth;
    GLint mHeight;
};

#endif /* defined(__LearnCG__SVDepthTexture__) */
