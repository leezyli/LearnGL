//
//  Camera.cpp
//  LearnCG
//
//  Created by leezy on 16/5/7.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "Camera.h"

Camera::Camera()
{
    mUpdate = true;
}

Camera::~Camera()
{
    
}

glm::vec3 Camera::GetUp() const
{
    return mUp;
}

glm::vec3 Camera::GetCenter() const
{
    return mCenter;
}

glm::vec3 Camera::GetEyePos() const
{
    return mEyePos;
}

glm::mat4 Camera::GetViewMat() const
{
    if ( mUpdate )
    {
        glm::vec3 n = glm::normalize(mEyePos - mCenter);
        glm::vec3 u = glm::normalize(glm::cross(mUp, n));
        glm::vec3 v = glm::normalize(glm::cross(n, u));
        glm::mat4 rot = glm::mat4(glm::vec4(u.x, v.x, n.x, 0),
                                  glm::vec4(u.y, v.y, n.y, 0),
                                  glm::vec4(u.z, v.z, n.z, 0),
                                  glm::vec4(0, 0, 0, 1.0));
        glm::mat4 trans = glm::translate(glm::mat4(1.0), -mEyePos);
        mViewMat =  rot * trans;
        mUpdate = false;
    }
  
    return mViewMat;
}

void Camera::LookAt(const glm::vec3 &aEyePos, const glm::vec3 &aCenter, const glm::vec3 &aUp)
{
    mUp = aUp;
    mEyePos = aEyePos;
    mCenter = aCenter;
    mUpdate = true;
   // mViewMat = glm::lookAt(aEyePos, aCenter, aUp);
}