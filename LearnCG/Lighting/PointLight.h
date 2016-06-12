//
//  PointLight.h
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__PointLight__
#define __LearnCG__PointLight__

#include <stdio.h>
#include "LightBase.h"

class PointLight: public LightBase
{
    typedef PointLight this_class;
public:
    PointLight();
    ~PointLight();
    
    float GetExp() const;
    float GetLinear() const;
    float GetConstant() const;
    
    void SetExp(float aExp);
    void SetLinear(float aLinear);
    void SetConstant(float aConstant);
protected:
    float mExp;
    float mLinear;
    float mConstant;
};

#endif /* defined(__LearnCG__PointLight__) */
