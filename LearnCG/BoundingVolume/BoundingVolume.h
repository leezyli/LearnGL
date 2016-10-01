//
//  BoundingVolume.h
//  LearnCG
//
//  Created by leezy on 16/7/25.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__BoundingVolume__
#define __LearnCG__BoundingVolume__

#include <stdio.h>
#include "LearnCG.h"

class Camera;
class Pipeline;

struct BoundingBox
{
    glm::vec4 near;
    glm::vec4 far;
    glm::vec4 left;
    glm::vec4 right;
    glm::vec4 top;
    glm::vec4 bottom;
};

struct BoundingSphere
{
    
};


class BoundingBoxDrawer
{
    typedef BoundingBoxDrawer this_class;
public:
    BoundingBoxDrawer();
    ~BoundingBoxDrawer();
    
    bool Make(BoundingBox& box);
    bool Render(Camera const& aCamera, Pipeline const& aPipeline);
protected:
    bool LoadShader();
private:
    GLuint mVAO;
    GLuint mProg;
};

#endif /* defined(__LearnCG__BoundingVolume__) */
