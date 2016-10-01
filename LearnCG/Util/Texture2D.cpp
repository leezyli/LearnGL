//
//  Texture2D.cpp
//  LearnCG
//
//  Created by leezy on 16/4/30.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "Texture2D.h"
#include "UtilCG.h"


bool Texture2D::mInitIL = false;

Texture2D::Texture2D()
{
    mTexID = 0;
    mTexSample = -1;
}

Texture2D::~Texture2D()
{
    if ( mTexID )
        glDeleteTextures(1, &mTexID);
}

bool Texture2D::Load(std::string const& aPath)
{
    mPath = aPath;
    
    if ( 0 != mTexID )
    {
        glDeleteTextures(1, &mTexID);
        mTexID = 0;
    }
    
    if ( LoadFromIL() )
        return true;
    
    mTexID = UtilCG::Texture::LoadTexture(mPath.c_str());
    if ( INVALID_HANDLE != mTexID )
        return true;
    
    return false;
}

void Texture2D::SetTextureWrap(GLenum eTextureWrap)
{
    if ( !mTexID )
        return;
    
    GLint nOldTexID = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &nOldTexID);
    
    glBindTexture(GL_TEXTURE_2D, mTexID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, eTextureWrap);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, eTextureWrap);
    glBindTexture(GL_TEXTURE_2D, nOldTexID);
}

GLuint Texture2D::Bind(GLenum eTexture2D)
{
    GLuint oldTexID = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast<GLint*>(&oldTexID));
    glActiveTexture(eTexture2D);
    glBindTexture(GL_TEXTURE_2D, mTexID);
    mTexSample = eTexture2D - GL_TEXTURE0;
    return oldTexID;
}

GLuint Texture2D::GetTex() const
{
    return mTexSample;
}

bool Texture2D::InitIL()
{
    ilInit();
    
    ILenum error = ilGetError();
    mInitIL = (error == IL_NO_ERROR);
    if ( !mInitIL )
    {
        return false;
    }
    
    return true;
}

bool Texture2D::LoadFromIL()
{
    if ( !mInitIL && !InitIL() )
        return false;
    
    ilImageWrapper image;
    if ( !image.Init() )
    {
        ILenum error = ilGetError();
        std::string strError = UtilCG::DevIL::ilGetErrorString(error);
        return false;
    }
    
    if ( !ilLoadImage(mPath.c_str()) )
    {
        GLenum error = ilGetError();
        std::string strError = UtilCG::DevIL::ilGetErrorString(error);
        return false;
    }
    
    //ILint size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
    //ILint bpp = ilGetInteger(IL_IMAGE_BPP);
    ILint type = ilGetInteger(IL_IMAGE_TYPE);
    ILint format = ilGetInteger(IL_IMAGE_FORMAT);
    ILint width = ilGetInteger(IL_IMAGE_WIDTH);
    ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
    ILint depth = ilGetInteger(IL_IMAGE_DEPTH);
    
    ILubyte* pData = ilGetData();
    GLenum typeGL = UtilCG::DevIL::ilGetGLtype(type);
    GLenum formatGL = UtilCG::DevIL::ilGetGLformat(format);
    GLenum internalFormatGL = UtilCG::DevIL::ilGetGLInternalFormat(format);
    
    glGenTextures(1, &mTexID);
    glBindTexture(GL_TEXTURE_2D, mTexID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    bool bSucc = true;
    glBindTexture(GL_TEXTURE_2D, mTexID);
    
    glTexImage2D(GL_TEXTURE_2D,
                 depth - 1,
                 internalFormatGL,
                 (GLint)width,
                 (GLint)height, 0,
                 formatGL, typeGL, pData);
    GLenum Error = glGetError();
    if ( GL_NO_ERROR != Error )
    {
        bSucc = false;
        printf("Texture2D::LoadFromIL!load failed: %x\n", Error);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return bSucc;
}

