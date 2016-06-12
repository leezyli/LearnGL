//
//  SMDepthTexture.h
//  LearnCG
//
//  Created by leezy on 16/5/18.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__SMDepthTexture__
#define __LearnCG__SMDepthTexture__

#include <stdio.h>
#include "LearnCG.h"

class SMDepthTexture
{
    typedef SMDepthTexture this_class;
public:
    SMDepthTexture();
    ~SMDepthTexture();
    
    bool Load(int aWidth, int aHeight);
    void BindForDraw();
    void BindForRead(GLenum eTexture); // e.g. GL_TEXTURE0
    GLuint GetTex() const;
protected:
    bool Init();
private:
    bool mInit;
    GLuint mFBO;
    GLuint mDepthTex;
    GLuint mTexSample;
    GLint mWidth;
    GLint mHeight;
};

#endif /* defined(__LearnCG__SMDepthTexture__) */
