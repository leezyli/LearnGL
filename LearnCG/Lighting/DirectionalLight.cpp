//
//  DirectionalLight.cpp
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "DirectionalLight.h"


DirectionalLight::DirectionalLight()
{
    
}

DirectionalLight::~DirectionalLight()
{
    
}

glm::vec3 DirectionalLight::GetCastPos() const
{
    return mCastPos;
}

void DirectionalLight::SetCastPos(const glm::vec3 &aPosition)
{
    mCastPos = aPosition;
}

