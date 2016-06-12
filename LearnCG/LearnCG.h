//
//  LearnCG.h
//  LearnCG
//
//  Created by leezy on 16/4/22.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef LearnCG_LearnCG_h
#define LearnCG_LearnCG_h

#include "stdio.h"
#include <OpenGL/gl3.h>
#include <utility>

#include <glfw/glfw3.h>

#define GLM_SWIZZLE
#define GLM_FORCE_CXX98
#include <glm/vec2.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>

#include "UtilCG.h"

extern std::string g_AssetsPath;

#define DEFAULT_WINDOW_WIDTH    700
#define DEFAULT_WINDOW_HEIGHT   700

#define RADIAN(x) (((x)*2*glm::pi<float>())/360)


#define GPU_Log(fmt, ...) printf(fmt, __VA_ARGS__)

extern void glfwErrorCallback_GPU(int error, const char* error_message);


#define MAX_BUF_SIZE 256
// GetObjectUniformLocation("gLights[%d].Color", i)
extern int32_t GetObjectUniformLocation(uint32_t program, const char* format, int index);

bool operator < (const glm::vec3& l, const glm::vec3& r);
bool operator == (const glm::vec3& l, const glm::vec3& r);
#endif
