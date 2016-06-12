//
//  Camera.h
//  LearnCG
//
//  Created by leezy on 16/5/7.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__Camera__
#define __LearnCG__Camera__

#include <stdio.h>
#include "LearnCG.h"

class Camera
{
    typedef Camera this_class;
public:
    Camera();
    ~Camera();
    
    glm::vec3 GetUp() const;
    glm::vec3 GetCenter() const;
    glm::vec3 GetEyePos() const;
    glm::mat4 GetViewMat() const;
    
    void LookAt(glm::vec3 const& aEyePos, glm::vec3 const& aCenter, glm::vec3 const& aUp);
protected:
    glm::vec3 mUp;
    glm::vec3 mEyePos;
    glm::vec3 mCenter;
    mutable bool mUpdate;
    mutable glm::mat4 mViewMat;
};

#endif /* defined(__LearnCG__Camera__) */
