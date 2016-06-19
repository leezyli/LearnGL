//
//  Sphere.h
//  LearnCG
//
//  Created by leezy on 16/6/18.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__Sphere__
#define __LearnCG__Sphere__

#include <stdio.h>
#include "LearnCG.h"
#include <vector>
#include <map>


enum ESPHERE_RENDER
{
    ESPHERE_RenderNormal,
    ESPHERE_RenderTriangleAdjancency
};

class Sphere
{
    typedef Sphere this_class;
public:
    Sphere();
    ~Sphere();
    
    void SetRadius(float aRad);
    void SetLantitude(int aLantitude);
    void SetLongitude(int aLongitude);
    void Render(ESPHERE_RENDER eRender = ESPHERE_RenderNormal);
protected:
    bool Init();
    
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
    };
    bool Init_RenderNormal(std::vector<Vertex> const& vertices, std::vector<GLushort> const& indices);
    bool Init_RenderTriangleAdjancency(std::vector<Vertex> const& vertices, std::vector<GLushort> const& indices);
    
    bool mInit;
    GLuint mVAO[2];
    GLuint mVBO;
    GLuint mIBO[2];
    
    float mR;
    GLint mLan;
    GLint mLong;
    GLint mFaces;
    GLint mAdjFaces;
    
    struct LessObj: std::binary_function<glm::vec3, glm::vec3, bool>
    {
        bool operator() (const glm::vec3& l, const glm::vec3& r) const
        {
            return l < r;
        }
    };
    
    typedef std::map<glm::vec3, GLushort, LessObj> UniqueMap;
    typedef std::map<std::pair<GLushort, GLushort>, GLushort> EdgeMap;
};

#endif /* defined(__LearnCG__Sphere__) */
