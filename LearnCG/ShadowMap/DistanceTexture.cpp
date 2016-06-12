//
//  DistanceTexture.cpp
//  LearnCG
//
//  Created by leezy on 16/5/21.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "DistanceTexture.h"


DistanceTexture::DistanceTexture()
{
    mInit = false;
    mFBO = 0;
    mCubeTex = 0;
    mDepthTex = 0;
    mTexSample = -1;
    mEdgeSize = 0;
}

DistanceTexture::~DistanceTexture()
{
    if ( mCubeTex )
        glDeleteTextures(1, &mCubeTex);
    if ( mDepthTex )
        glDeleteTextures(1, &mDepthTex);
    if ( mFBO )
        glDeleteFramebuffers(1, &mFBO);
}

bool DistanceTexture::Load(int aEdgeSize)
{
    if ( mEdgeSize == aEdgeSize )
        return true;
    
    if ( !mInit )
        mInit = Init();
    
    if ( !mInit )
        return false;
    
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, aEdgeSize, aEdgeSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeTex);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_R32F, aEdgeSize, aEdgeSize, 0, GL_RED, GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_R32F, aEdgeSize, aEdgeSize, 0, GL_RED, GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_R32F, aEdgeSize, aEdgeSize, 0, GL_RED, GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_R32F, aEdgeSize, aEdgeSize, 0, GL_RED, GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_R32F, aEdgeSize, aEdgeSize, 0, GL_RED, GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_R32F, aEdgeSize, aEdgeSize, 0, GL_RED, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTex, 0);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( GL_FRAMEBUFFER_COMPLETE != status ) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        printf("DistanceTexture::Load!fbo failed: %x", status);
        return false;
    }
    
    mEdgeSize = aEdgeSize;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void DistanceTexture::BindForDraw(GLenum eCubeFace)
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, eCubeFace, mCubeTex, 0);
}

void DistanceTexture::BindForRead(GLenum eTexture)
{
    if ( !mFBO )
        return;
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glActiveTexture(eTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeTex);
    mTexSample = eTexture - GL_TEXTURE0;
}

GLuint DistanceTexture::GetTex() const
{
    return mTexSample;
}

bool DistanceTexture::Init()
{
    glGenTextures(1, &mDepthTex);
    glBindTexture(GL_TEXTURE_2D, mDepthTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenTextures(1, &mCubeTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeTex);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    glGenFramebuffers(1, &mFBO);
    return true;
}
