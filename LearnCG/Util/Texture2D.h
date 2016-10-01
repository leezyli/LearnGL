//
//  Texture2D.h
//  LearnCG
//
//  Created by leezy on 16/4/30.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __GPUGems__Texture2D__
#define __GPUGems__Texture2D__

#include <stdio.h>
#include "OpenGL/gl3.h"
#include <string>
#include <DevIL/il.h>

class Texture2D
{
public:
    Texture2D();
    ~Texture2D();
    
    bool Load(std::string const& aPath);
    void SetTextureWrap(GLenum eTextureWrap);
    GLuint Bind(GLenum eTexture2D); // e.g. GL_TEXTURE0
    GLuint GetTex() const;
protected:
    bool LoadFromIL();
    static bool InitIL();
    static bool mInitIL;
private:
    class ilImageWrapper
    {
        ILint mImage;
    public:
        ilImageWrapper()
        {
            mImage = 0;
        }
        
        ~ilImageWrapper()
        {
            if ( mImage )
            {
                ilDeleteImage(mImage);
            }
        }
        bool Init()
        {
            mImage = ilGenImage();
            if ( !mImage )
            {
                return false;
            }
            ilBindImage(mImage);
            return true;
        }
        
        ILint GetImage() const { return mImage; }
    };
    
    GLuint mTexID;
    GLuint mTexSample;
    std::string mPath;
};

#endif /* defined(__GPUGems__Texture2D__) */
