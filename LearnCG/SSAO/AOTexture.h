//
//  AOTexture.h
//  LearnCG
//
//  Created by leezy on 16/7/3.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__AOTexture__
#define __LearnCG__AOTexture__

#include <stdio.h>
#include "LearnCG.h"

class AOTexture
{
    typedef AOTexture this_class;
public:
    AOTexture();
    ~AOTexture();
    
    bool Load(int width, int height);
    void BindForDraw();
    void BindForRead(GLenum eTexture);
    GLuint GetTex() const;
protected:
    bool Init();
private:
    bool mInit;
    GLuint mFBO;
    GLuint mAOTex;
    GLuint mTexSample;
    GLint mWidth;
    GLint mHeight;
};

#endif /* defined(__LearnCG__AOTexture__) */
