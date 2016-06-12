//
//  DirectionalLight.h
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__DirectionalLight__
#define __LearnCG__DirectionalLight__

#include <stdio.h>
#include "LightBase.h"

class DirectionalLight: public LightBase
{
    typedef DirectionalLight this_class;
public:
    DirectionalLight();
    ~DirectionalLight();
    
    glm::vec3 GetCastPos() const;
    void SetCastPos(glm::vec3 const& aPosition);
protected:
    glm::vec3 mCastPos;
};

#endif /* defined(__LearnCG__DirectionalLight__) */
