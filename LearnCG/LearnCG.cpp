//
//  LearnCG.cpp
//  LearnCG
//
//  Created by leezy on 16/5/12.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include <stdio.h>
#include "LearnCG.h"

std::string g_AssetsPath = "/Users/leezyli/Documents/VMShared/GitHub/LearnCG/LearnCG/";

void glfwErrorCallback_GPU(int error, const char* error_message)
{
    GPU_Log("glfw.error: %d[%s]", error, error_message);
}

int32_t GetObjectUniformLocation(uint32_t program, const char* format, int index)
{
    char bufstream[MAX_BUF_SIZE] = {0};
    sprintf(bufstream, format, index);
    int32_t indice = glGetUniformLocation(program, bufstream);
    assert(indice != 0);
    return indice;
}

bool operator < (const glm::vec3& l, const glm::vec3& r)
{
    if ( l.x < r.x ) return true;
    else if ( l.x == r.x ) {
        if ( l.y < r.y ) return true;
        else if ( l.y == r.y ) {
            if ( l.z < r.z ) return true;
        }
    }
    return false;
}

bool operator == (const glm::vec3& l, const glm::vec3& r)
{
    if ( l.x == r.x && l.y == r.y && l.z == r.z ) return true;
    return false;
}
