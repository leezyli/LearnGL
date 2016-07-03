//
//  AODepth.cpp
//  LearnCG
//
//  Created by leezy on 16/7/3.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "AODepth.h"

AODepth::AODepth()
{
    mInit = false;
    mFBO = 0;
    mDepthTex = 0;
    mWidth = 0;
    mHeight = 0;
    mTexSample = INVALID_HANDLE;
}

AODepth::~AODepth()
{
    if ( mFBO )
        glDeleteFramebuffers(1, &mFBO);
    if ( mDepthTex )
        glDeleteTextures(1, &mDepthTex);
}

bool AODepth::Load(int width, int height)
{
    if ( mWidth == width && mHeight == height )
        return true;
    
    if ( !mInit )
    {
        mInit = Init();
        if ( !mInit )
            return false;
    }
    
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthTex, 0);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( GL_FRAMEBUFFER_COMPLETE != status )
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        printf("AODepth::Load failed: %x\n", status);
        return false;
    }
    
    mWidth = width;
    mHeight = height;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

bool AODepth::Init()
{
    glGenTextures(1, &mDepthTex);
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenFramebuffers(1, &mFBO);
    return true;
}

void AODepth::BindForDraw()
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
}

void AODepth::BindForRead(GLenum eTexture)
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    glActiveTexture(eTexture);
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    mTexSample = eTexture - GL_TEXTURE0;
}

GLuint AODepth::GetTex() const
{
    return mTexSample;
}