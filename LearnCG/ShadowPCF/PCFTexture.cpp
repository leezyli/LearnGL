//
//  PCFTexture.cpp
//  LearnCG
//
//  Created by leezy on 16/6/26.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "PCFTexture.h"

PCFTexture::PCFTexture()
{
    mFBO = 0;
    mDepthTex = 0;
    mWidth = 0;
    mHeight = 0;
    mInit = false;
    mTexSample = 0;
}

PCFTexture::~PCFTexture()
{
    if ( mFBO )
        glDeleteFramebuffers(1, &mFBO);
    if ( mDepthTex )
        glDeleteTextures(1, &mDepthTex);
}

bool PCFTexture::Load(int width, int height)
{
    if ( mWidth == width && mHeight == height )
        return true;
    
    if ( !mInit ) {
        mInit = Init();
        if ( !mInit )
            return false;
    }
    
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthTex, 0);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( GL_FRAMEBUFFER_COMPLETE != status ) {
        printf("PCFTexture::Load!framebuffer load failed: %x", status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    
    mWidth = width;
    mHeight = height;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void PCFTexture::BindForDraw()
{
    if ( !mFBO )
        return;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
}

void PCFTexture::BindForRead(GLenum eTexture)
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    glActiveTexture(eTexture);
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    mTexSample = eTexture - GL_TEXTURE0;
}

GLuint PCFTexture::GetTex() const
{
    return mTexSample;
}

GLuint PCFTexture::GetWidth() const
{
    return mWidth;
}

GLuint PCFTexture::GetHeight() const
{
    return mHeight;
}

bool PCFTexture::Init()
{
    glGenTextures(1, &mDepthTex);
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenFramebuffers(1, &mFBO);
    return true;
}
