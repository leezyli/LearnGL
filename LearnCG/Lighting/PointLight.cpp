//
//  PointLight.cpp
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "PointLight.h"


PointLight::PointLight()
{
    
}

PointLight::~PointLight()
{
    
}

float PointLight::GetExp() const
{
    return mExp;
}

float PointLight::GetLinear() const
{
    return mLinear;
}

float PointLight::GetConstant() const
{
    return mConstant;
}

void PointLight::SetExp(float aExp)
{
    mExp = aExp;
}

void PointLight::SetLinear(float aLinear)
{
    mLinear = aLinear;
}

void PointLight::SetConstant(float aConstant)
{
    mConstant = aConstant;
}
