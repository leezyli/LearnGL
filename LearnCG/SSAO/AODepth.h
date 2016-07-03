//
//  AODepth.h
//  LearnCG
//
//  Created by leezy on 16/7/3.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__AODepth__
#define __LearnCG__AODepth__

#include <stdio.h>
#include "LearnCG.h"

class AODepth
{
    typedef AODepth this_class;
public:
    AODepth();
    ~AODepth();
    
    bool Load(int width, int height);
    void BindForDraw();
    void BindForRead(GLenum eTexture);
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

#endif /* defined(__LearnCG__AODepth__) */
