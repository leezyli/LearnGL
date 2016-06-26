//
//  PCFTexture.h
//  LearnCG
//
//  Created by leezy on 16/6/26.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__PCFTexture__
#define __LearnCG__PCFTexture__

#include <stdio.h>
#include "LearnCG.h"

class PCFTexture
{
    typedef PCFTexture this_class;
public:
    PCFTexture();
    ~PCFTexture();
    
    bool Load(int width, int height);
    void BindForDraw();
    void BindForRead(GLenum eTexture);
    GLuint GetTex() const;
    GLuint GetWidth() const;
    GLuint GetHeight() const;
protected:
    bool Init();
    bool mInit;
    GLuint mFBO;
    GLuint mDepthTex;
    GLuint mTexSample;
    int mWidth;
    int mHeight;
};

#endif /* defined(__LearnCG__PCFTexture__) */
