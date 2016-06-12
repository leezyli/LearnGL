//
//  DistanceTexture.h
//  LearnCG
//
//  Created by leezy on 16/5/21.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__DistanceTexture__
#define __LearnCG__DistanceTexture__

#include <stdio.h>
#include "LearnCG.h"


class DistanceTexture
{
    typedef DistanceTexture this_class;
public:
    DistanceTexture();
    ~DistanceTexture();
    
    bool Load(int aEdgeSize);
    void BindForDraw(GLenum eCubeFace);
    void BindForRead(GLenum eTexture); // e.g. GL_TEXTURE0
    GLuint GetTex() const;
protected:
    bool Init();
private:
    bool mInit;
    GLuint mFBO;
    GLuint mDepthTex;
    GLuint mCubeTex;
    GLuint mTexSample;
    GLint mEdgeSize;
};

#endif /* defined(__LearnCG__DistanceTexture__) */
