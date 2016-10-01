//
//  UtilCG.h
//  LearnCG
//
//  Created by leezy on 16/4/29.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __GPUGems__UtilCG__
#define __GPUGems__UtilCG__

#include <stdio.h>
#include <string>

#ifndef INVALID_HANDLE
#define INVALID_HANDLE 0xFFFFFFFF
#endif//INVALID_HANDLE

namespace UtilCG
{
    namespace Shader
    {
        uint32_t LoadShaders(const char* const vs, const char* const fs, const char* const gs = NULL);
    }
    
    namespace Texture
    {
        uint32_t LoadTexture(const char* const path);
        uint32_t LoadCubeTexture(const char* const path);
    }
    
    namespace DevIL
    {
        uint32_t ilGetGLtype(uint32_t type);
        uint32_t ilGetGLformat(uint32_t format);
        uint32_t ilGetGLInternalFormat(uint32_t format);
        std::string ilGetErrorString(uint32_t error);
    }
}

#endif /* defined(__GPUGems__UtilCG__) */
