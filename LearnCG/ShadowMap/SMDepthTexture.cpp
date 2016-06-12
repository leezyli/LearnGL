//
//  DepthTexture.cpp
//  LearnCG
//
//  Created by leezy on 16/5/18.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "SMDepthTexture.h"


SMDepthTexture::SMDepthTexture()
{
    mInit = false;
    mFBO = 0;
    mDepthTex = 0;
    mWidth = 0;
    mHeight = 0;
}

SMDepthTexture::~SMDepthTexture()
{
    if ( mFBO )
        glDeleteFramebuffers(1, &mFBO);
    if ( mDepthTex )
        glDeleteTextures(1, &mDepthTex);
}

bool SMDepthTexture::Load(int aWidth, int aHeight)
{
    if ( mWidth == aWidth && mHeight == aHeight )
        return true;
    
    if ( !mInit )
        mInit = Init();
    
    if ( !mInit )
        return false;
    
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, aWidth, aHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTex, 0);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( GL_FRAMEBUFFER_COMPLETE != status )
    {
        printf("DepthTexture::Load!framebuffer load failed: %x", status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    
    mWidth = aWidth;
    mHeight = aHeight;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void SMDepthTexture::BindForDraw()
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
}

void SMDepthTexture::BindForRead(GLenum eTexture)
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    glActiveTexture(eTexture);
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    
    mTexSample = eTexture - GL_TEXTURE0;
}

GLuint SMDepthTexture::GetTex() const
{
    return mTexSample;
}

bool SMDepthTexture::Init()
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
