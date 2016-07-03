//
//  AOTexture.cpp
//  LearnCG
//
//  Created by leezy on 16/7/3.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "AOTexture.h"

AOTexture::AOTexture()
{
    mInit = false;
    mFBO = 0;
    mAOTex = 0;
    mWidth = 0;
    mHeight = 0;
    mTexSample = INVALID_HANDLE;
}

AOTexture::~AOTexture()
{
    if ( mFBO )
        glDeleteFramebuffers(1, &mFBO);
    if ( mAOTex )
        glDeleteTextures(1, &mAOTex);
}

bool AOTexture::Load(int width, int height)
{
    if ( mWidth == width && mHeight == height )
        return true;
    
    if ( !mInit )
    {
        mInit = Init();
        if ( !mInit )
            return false;
    }
    
    glBindTexture(GL_TEXTURE_2D, mAOTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_NONE);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mAOTex, 0);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( GL_FRAMEBUFFER_COMPLETE != status )
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        printf("AOTexture::Load failed: %x\n", status);
        return false;
    }
    
    mWidth = width;
    mHeight = height;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

bool AOTexture::Init()
{
    glGenTextures(1, &mAOTex);
    glBindTexture(GL_TEXTURE_2D, mAOTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenFramebuffers(1, &mFBO);
    return true;
}

void AOTexture::BindForDraw()
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
}

void AOTexture::BindForRead(GLenum eTexture)
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    glActiveTexture(eTexture);
    glBindTexture(GL_TEXTURE_2D, mAOTex);
    mTexSample = eTexture - GL_TEXTURE0;
}

GLuint AOTexture::GetTex() const
{
    return mTexSample;
}

