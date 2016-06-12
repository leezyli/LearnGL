//
//  Pipeline.cpp
//  LearnCG
//
//  Created by leezy on 16/5/7.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "Pipeline.h"


Pipeline::Pipeline()
{

}

Pipeline::~Pipeline()
{
    
}

glm::mat4 Pipeline::GetProjMat() const
{
    return mProjMat;
}

void Pipeline::SetOrth(GLfloat fLeft, GLfloat fRight, GLfloat fBottom, GLfloat fTop, GLfloat fNear, GLfloat fFar)
{
    mProjMat = glm::ortho(fLeft, fRight, fBottom, fTop, fNear, fFar); // near, far以距离表示(非坐标)
}

void Pipeline::SetPerspective(GLfloat fFov, GLfloat fAspect, GLfloat fNear, GLfloat fFar)
{
    mProjMat = glm::perspective(RADIAN(fFov), fAspect, fNear, fFar); // near, far以距离表示(非坐标)
}

void Pipeline::SetPerspective(const glm::mat4 &aProjMat)
{
    mProjMat = aProjMat;
}
