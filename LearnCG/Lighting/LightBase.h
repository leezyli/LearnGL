//
//  LightBase.h
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__LightBase__
#define __LearnCG__LightBase__

#include <stdio.h>
#include <utility>

#define GLM_SWIZZLE
#define GLM_FORCE_CXX98
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class LightBase
{
    typedef LightBase this_class;
public:
    LightBase();
    ~LightBase();
    
    glm::vec4 GetLightColor() const;
    glm::vec4 GetAmbientColor() const;
    glm::vec3 GetLightPos() const;
    
    void MoveTo(glm::vec3 const& aPosition);
    void SetLightPos(glm::vec3 const& aLightPos);
    void SetLightColor(glm::vec4 const& aColor);
    void SetLightIntensity(float aIntensity);
    void SetAmbientColor(glm::vec4 const& aColor);
    void SetAmbientIntensity(float aIntensity);
    
protected:
    glm::vec3 mLightPos;
    glm::vec3 mTranslate;
    glm::vec4 mLightColor;
    glm::vec4 mAmbientColor;
    float mLightIntensity;
    float mAmbientIntensity;
};

#endif /* defined(__LearnCG__LightBase__) */
