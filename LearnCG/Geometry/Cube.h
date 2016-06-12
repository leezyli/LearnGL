//
//  Cube.h
//  LearnCG
//
//  Created by leezy on 16/6/4.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__Cube__
#define __LearnCG__Cube__

#include <stdio.h>
#include "LearnCG.h"
#include <map>

enum ECUBE_RENDER
{
    ECUBE_RenderNormal,
    ECUBE_RenderTriangleAdjacency
};

class Cube
{
public:
    Cube();
    ~Cube();
    void Render(ECUBE_RENDER eRender = ECUBE_RenderNormal);
protected:
    bool Init();
    
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
    };
    bool mInit;
    GLuint mVAO[2];
    GLuint mVBO;
    GLuint mIBO[2];
    
    typedef std::pair<glm::vec3, glm::vec3> TSVer;
    struct LessObj: std::binary_function<TSVer, TSVer, bool>
    {
        bool operator()(const TSVer& l, const TSVer& r) const
        {
            if ( l.first < r.first ) return true;
            if ( l.first == r.first ) {
                if ( l.second < r.second ) return true;
            }
            return false;
        }
    };
    typedef std::map<TSVer, GLushort, LessObj> EdgeMap;
};

#endif /* defined(__LearnCG__Cube__) */
