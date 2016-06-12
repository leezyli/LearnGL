//
//  SpotLight.cpp
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "SpotLight.h"
#include "LearnCG.h"


SpotLight::SpotLight()
{
    
}

SpotLight::~SpotLight()
{
    
}

float SpotLight::GetCastAngel() const
{
    return mAngel;
}

glm::vec3 SpotLight::GetCastPos() const
{
    return mCastPos;
}

void SpotLight::SetCastAngle(float aAngle)
{
    mAngel = RADIAN(aAngle);
}

void SpotLight::SetCastPos(const glm::vec3 &aPosition)
{
    mCastPos = aPosition;
}
