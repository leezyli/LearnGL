//
//  LightBase.cpp
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "LightBase.h"
#include <glm/gtc/matrix_transform.hpp>

LightBase::LightBase()
{
    
}

LightBase::~LightBase()
{
    
}

glm::vec4 LightBase::GetLightColor() const
{
    glm::vec4 LightColor = mLightColor * mLightIntensity;
    return LightColor;
}

glm::vec4 LightBase::GetAmbientColor() const
{
    glm::vec4 AmbientColor = mAmbientColor * mAmbientIntensity;
    return AmbientColor;
}

glm::vec3 LightBase::GetLightPos() const
{
    glm::mat4 TransMat = glm::translate(glm::mat4(1), mTranslate);
    glm::vec4 Result = TransMat * glm::vec4(mLightPos, 1);
    return Result.xyz();
}

void LightBase::MoveTo(const glm::vec3 &aPosition)
{
    mTranslate = aPosition;
}

void LightBase::SetLightPos(const glm::vec3 &aLightPos)
{
    mLightPos = aLightPos;
}

void LightBase::SetLightColor(const glm::vec4 &aColor)
{
    mLightColor = aColor;
}

void LightBase::SetLightIntensity(float aIntensity)
{
    mLightIntensity = aIntensity;
}

void LightBase::SetAmbientColor(const glm::vec4 &aColor)
{
    mAmbientColor = aColor;
}

void LightBase::SetAmbientIntensity(float aIntensity)
{
    mAmbientIntensity = aIntensity;
}
