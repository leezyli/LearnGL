//
//  UtilCG.cpp
//  LearnCG
//
//  Created by leezy on 16/4/29.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "UtilCG.h"
#include <OpenGL/gl3.h>
#include <fstream>
#include <DevIL/il.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>

#define MAX_LOG_NUM 512

namespace UtilCG
{
    namespace Internal
    {
        uint32_t LoadSource(const char* const src, uint32_t mode);
        uint32_t CompileSource(uint32_t hShader);
    }
}

uint32_t UtilCG::Internal::LoadSource(const char *const src, uint32_t mode)
{
    std::ifstream in(src);
    if ( !in.is_open() )
        return INVALID_HANDLE;
    
    in.seekg(0, std::ios_base::end);
    std::ifstream::pos_type totalSize = in.tellg();
    
    in.seekg(0, std::ios_base::beg);
    char* const buf = new char[totalSize];
    in.read(buf, totalSize);
    
    uint32_t hShader = glCreateShader(mode);
    if ( 0 != hShader )
    {
        GLchar* srcCodes[1] = {buf};
        GLint readSize = static_cast<GLint>(totalSize);
        glShaderSource(hShader, 1, &srcCodes[0], &readSize);
    }
    
    GLenum error = glGetError();
    if ( GL_NO_ERROR != error )
    {
        //GPU_Log("glShaderSource failed: %x", error);
        glDeleteShader(hShader);
        hShader = INVALID_HANDLE;
    }
    
    delete [] buf;
    return hShader;
}

uint32_t UtilCG::Internal::CompileSource(uint32_t hShader)
{
    if ( !hShader || INVALID_HANDLE == hShader )
        return INVALID_HANDLE;
    
    glCompileShader(hShader);
    
    int status = GL_FALSE;
    glGetShaderiv(hShader, GL_COMPILE_STATUS, &status);
    if ( GL_TRUE != status )
    {
        GLsizei len = 0;
        GLchar log[MAX_LOG_NUM] = {0};
        glGetShaderInfoLog(hShader, MAX_LOG_NUM, &len, log);
        printf("glCompileShader failed: %s", log);
        glDeleteShader(hShader);
        hShader = INVALID_HANDLE;
    }
    
    return hShader;
}

uint32_t UtilCG::Shader::LoadShaders(const char *const vs, const char *const fs, const char *const gs)
{
    uint32_t hVShader = 0;
    hVShader = UtilCG::Internal::LoadSource(vs, GL_VERTEX_SHADER);
    hVShader = UtilCG::Internal::CompileSource(hVShader);
    if ( INVALID_HANDLE == hVShader )
        return INVALID_HANDLE;
    
    uint32_t hFShader = 0;
    hFShader = UtilCG::Internal::LoadSource(fs, GL_FRAGMENT_SHADER);
    hFShader = UtilCG::Internal::CompileSource(hFShader);
    if ( INVALID_HANDLE == hFShader )
        return INVALID_HANDLE;
    
    uint32_t hGShader = 0;
    if ( gs != NULL )
    {
        hGShader = UtilCG::Internal::LoadSource(gs, GL_GEOMETRY_SHADER);
        hGShader = UtilCG::Internal::CompileSource(hGShader);
        if ( INVALID_HANDLE == hGShader )
            return INVALID_HANDLE;
    }
    
    uint32_t hProgram = glCreateProgram();
    if ( 0 != hProgram )
    {
        glAttachShader(hProgram, hVShader);
        glAttachShader(hProgram, hFShader);
        if ( hGShader )
        {
            glAttachShader(hProgram, hGShader);
        }
        
        glLinkProgram(hProgram);
        
        int status = GL_FALSE;
        glGetProgramiv(hProgram, GL_LINK_STATUS, &status);
        if ( GL_TRUE != status )
        {
            GLsizei len = 0;
            GLchar log[MAX_LOG_NUM] = {0};
            glGetProgramInfoLog(hProgram, MAX_LOG_NUM, &len, log);
            printf("glLinkProgram failed: %s", log);
            glDeleteProgram(hProgram);
            hProgram = INVALID_HANDLE;
        }
    }
    
    glDeleteShader(hVShader);
    glDeleteShader(hFShader);
    glDeleteShader(hGShader);
    
    return hProgram;
}

uint32_t UtilCG::Texture::LoadTexture(const char *const aPath)
{
    CFStringRef Path = CFStringCreateWithCString(kCFAllocatorDefault, aPath, kCFStringEncodingASCII);
    CFURLRef Url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, Path, kCFURLPOSIXPathStyle, false);
    CGImageSourceRef Source = CGImageSourceCreateWithURL(Url, NULL);
    
    CGImageRef pImage = CGImageSourceCreateImageAtIndex(Source, 0, NULL);
    if ( !pImage )
    {
        CFRelease(Url);
        CFRelease(Path);
        CFRelease(Source);
        return INVALID_HANDLE;
    }
    
    CGDataProviderRef pDataProvider = CGImageGetDataProvider(pImage);
    if ( pDataProvider )
    {
        size_t width = CGImageGetWidth(pImage);
        size_t height = CGImageGetHeight(pImage);
        //size_t bytesPerRow = CGImageGetBytesPerRow(pImage);
        //size_t bitsPerPixel = CGImageGetBitsPerPixel(pImage);
        //size_t bitsPerComponent = CGImageGetBitsPerComponent(pImage);
        CFDataRef DataRef = CGDataProviderCopyData(pDataProvider);
        const UInt8* pData = CFDataGetBytePtr(DataRef);
        
        uint32_t hTexID = 0;
        glGenTextures(1, &hTexID);
        glBindTexture(GL_TEXTURE_2D, hTexID);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLint)width, (GLint)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        CFRelease(Url);
        CFRelease(Path);
        CFRelease(Source);
        CGImageRelease(pImage);
      //  CGDataProviderRelease(pDataProvider);
        return hTexID;
    }
    
    CFRelease(Url);
    CFRelease(Path);
    CFRelease(Source);
    CGImageRelease(pImage);
    CGDataProviderRelease(pDataProvider);
    
    return INVALID_HANDLE;
}

uint32_t UtilCG::DevIL::ilGetGLtype(uint32_t type)
{
    uint32_t eType = GL_UNSIGNED_BYTE;
    switch (type) {
        case IL_BYTE:
            eType = GL_BYTE;
            break;
        case IL_UNSIGNED_BYTE:
            eType = GL_UNSIGNED_BYTE;
            break;
        case IL_SHORT:
            eType = GL_SHORT;
            break;
        case IL_UNSIGNED_SHORT:
            eType = GL_UNSIGNED_SHORT;
            break;
        case IL_INT:
            eType = GL_INT;
            break;
        case IL_UNSIGNED_INT:
            eType = GL_UNSIGNED_INT;
            break;
        case IL_FLOAT:
            eType = GL_FLOAT;
            break;
        case IL_DOUBLE:
            eType = GL_DOUBLE;
            break;
        case IL_HALF:
            eType = GL_HALF_FLOAT;
            break;
        default:
            break;
    }
    return eType;
}

uint32_t UtilCG::DevIL::ilGetGLformat(uint32_t format)
{
    uint32_t eFormat = GL_RGBA;
    switch (format) {
        case IL_RGB:
            eFormat = GL_RGB;
            break;
        case IL_RGBA:
        case IL_BGRA:
            eFormat = GL_RGBA;
            break;
        case IL_BGR:
            eFormat = GL_BGR;
            break;
        default:
            break;
    }
    return eFormat;
}

uint32_t UtilCG::DevIL::ilGetGLInternalFormat(uint32_t format)
{
    uint32_t eInternalFormat = GL_RGBA;
    switch (format) {
        case IL_RGB:
        case IL_BGR:
            eInternalFormat = GL_RGB;
            break;
        case IL_RGBA:
        case IL_BGRA:
            eInternalFormat = GL_RGBA;
            break;
        default:
            break;
    }
    return eInternalFormat;
}

std::string UtilCG::DevIL::ilGetErrorString(uint32_t error)
{
    std::string strError;
    switch ( error ) {
        case IL_INVALID_ENUM:
            strError = "IL_INVALID_ENUM";
            break;
        case IL_OUT_OF_MEMORY:
            strError = "IL_OUT_OF_MEMORY";
            break;
        case IL_FORMAT_NOT_SUPPORTED:
            strError = "IL_FORMAT_NOT_SUPPORTED";
            break;
        case IL_INTERNAL_ERROR:
            strError = "IL_INTERNAL_ERROR";
            break;
        case IL_INVALID_VALUE:
            strError = "IL_INVALID_VALUE";
            break;
        case IL_ILLEGAL_OPERATION:
            strError = "IL_ILLEGAL_OPERATION";
            break;
        case IL_ILLEGAL_FILE_VALUE:
            strError = "IL_ILLEGAL_FILE_VALUE";
            break;
        case IL_INVALID_FILE_HEADER:
            strError = "IL_INVALID_FILE_HEADER";
            break;
        case IL_INVALID_PARAM:
            strError = "IL_INVALID_PARAM";
            break;
        case IL_COULD_NOT_OPEN_FILE:
            strError = "IL_COULD_NOT_OPEN_FILE";
            break;
        case IL_INVALID_EXTENSION:
            strError = "IL_INVALID_EXTENSION";
            break;
        case IL_FILE_ALREADY_EXISTS:
            strError = "IL_FILE_ALREADY_EXISTS";
            break;
        case IL_OUT_FORMAT_SAME:
            strError = "IL_OUT_FORMAT_SAME";
            break;
        case IL_STACK_OVERFLOW:
            strError = "IL_STACK_OVERFLOW";
            break;
        case IL_STACK_UNDERFLOW:
            strError = "IL_STACK_UNDERFLOW";
            break;
        case IL_INVALID_CONVERSION:
            strError = "IL_INVALID_CONVERSION";
            break;
        case IL_BAD_DIMENSIONS:
            strError = "IL_BAD_DIMENSIONS";
            break;
        case IL_FILE_READ_ERROR:
            //case IL_FILE_WRITE_ERROR:
            strError = "IL_FILE_READ_ERROR|IL_FILE_WRITE_ERROR";
            break;
        case IL_LIB_GIF_ERROR:
            strError = "IL_LIB_GIF_ERROR";
            break;
        case IL_LIB_JPEG_ERROR:
            strError = "IL_LIB_JPEG_ERROR";
            break;
        case IL_LIB_PNG_ERROR:
            strError = "IL_LIB_PNG_ERROR";
            break;
        case IL_LIB_TIFF_ERROR:
            strError = "IL_LIB_TIFF_ERROR";
            break;
        case IL_LIB_MNG_ERROR:
            strError = "IL_LIB_MNG_ERROR";
            break;
        case IL_LIB_JP2_ERROR:
            strError = "IL_LIB_JP2_ERROR";
            break;
        case IL_LIB_EXR_ERROR:
            strError = "IL_LIB_EXR_ERROR";
            break;
        default:
            break;
    }
    return strError;
}
