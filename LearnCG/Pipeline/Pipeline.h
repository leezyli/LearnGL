//
//  Pipeline.h
//  LearnCG
//
//  Created by leezy on 16/5/7.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__Pipeline__
#define __LearnCG__Pipeline__

#include <stdio.h>
#include "LearnCG.h"

class Camera;

class Pipeline
{
    typedef Pipeline this_class;
public:
    Pipeline();
    ~Pipeline();
    
    glm::mat4 GetProjMat() const;
    void SetOrth(GLfloat fLeft, GLfloat fRight, GLfloat fBottom, GLfloat fTop, GLfloat fNear, GLfloat fFar);
    void SetPerspective(GLfloat fFov, GLfloat fAspect, GLfloat fNear, GLfloat fFar);
    void SetPerspective(glm::mat4 const& aProjMat);
protected:
    mutable glm::mat4 mProjMat;
};

#endif /* defined(__LearnCG__Pipeline__) */
