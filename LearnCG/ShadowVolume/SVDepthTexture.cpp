//
//  DepthTexture.cpp
//  LearnCG
//
//  Created by leezy on 16/6/3.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "SVDepthTexture.h"

SVDepthTexture::SVDepthTexture()
{
    mInit = false;
    mFBO = 0;
    mDepthTex = 0;
    mSampler = -1;
    mWidth = 0;
    mHeight = 0;
}

SVDepthTexture::~SVDepthTexture()
{
    if ( mFBO )
        glDeleteFramebuffers(1, &mFBO);
    if ( mDepthTex )
        glDeleteTextures(1, &mDepthTex);
}

bool SVDepthTexture::Load(int width, int height)
{
    if ( mWidth == width && mHeight == height )
        return true;
    
    if ( !mInit ) {
        mInit = InitFBO();
        if ( !mInit )
            return false;
    }
    
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTex, 0);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( GL_FRAMEBUFFER_COMPLETE != status ) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        printf("DepthTexture::Load failed: %x\n", status);
        return false;
    }
    
    mWidth = width;
    mHeight = height;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void SVDepthTexture::BindForDraw()
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
}

void SVDepthTexture::BindForRead(GLenum eTexture)
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    glActiveTexture(eTexture);
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    mSampler = eTexture - GL_TEXTURE0;
}

GLint SVDepthTexture::GetTex() const
{
    return mSampler;
}

bool SVDepthTexture::InitFBO()
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
