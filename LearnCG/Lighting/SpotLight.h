//
//  SpotLight.h
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__SpotLight__
#define __LearnCG__SpotLight__

#include <stdio.h>
#include "PointLight.h"

class SpotLight: public PointLight
{
    typedef SpotLight this_class;
public:
    SpotLight();
    ~SpotLight();
    
    float GetCastAngel() const;
    glm::vec3 GetCastPos() const;
    
    void SetCastAngle(float aAngle);
    void SetCastPos(glm::vec3 const& aPosition);
protected:
    float mAngel;
    glm::vec3 mCastPos;
};

#endif /* defined(__LearnCG__SpotLight__) */
